#include "api_client_ivc.hpp"
#include "barretenberg/api/file_io.hpp"
#include "barretenberg/api/init_srs.hpp"
#include "barretenberg/api/log.hpp"
#include "barretenberg/api/write_prover_output.hpp"
#include "barretenberg/client_ivc/mock_circuit_producer.hpp"
#include "barretenberg/common/map.hpp"
#include "barretenberg/common/throw_or_abort.hpp"
#include "barretenberg/common/try_catch_shim.hpp"
#include "barretenberg/dsl/acir_format/acir_to_constraint_buf.hpp"
#include "barretenberg/dsl/acir_format/ivc_recursion_constraint.hpp"
#include "libdeflate.h"
#include <stdexcept>

namespace bb {

acir_format::WitnessVector witness_map_to_witness_vector(std::map<std::string, std::string> const& witness_map)
{
    acir_format::WitnessVector wv;
    size_t index = 0;
    for (auto& e : witness_map) {
        uint64_t value = std::stoull(e.first);
        // ACIR uses a sparse format for WitnessMap where unused witness indices may be left unassigned.
        // To ensure that witnesses sit at the correct indices in the `WitnessVector`, we fill any indices
        // which do not exist within the `WitnessMap` with the dummy value of zero.
        while (index < value) {
            wv.push_back(fr(0));
            index++;
        }
        wv.push_back(fr(uint256_t(e.second)));
        index++;
    }
    return wv;
}

std::vector<uint8_t> decompress(const void* bytes, size_t size)
{
    std::vector<uint8_t> content;
    // initial size guess
    content.resize(1024ULL * 128ULL);
    for (;;) {
        auto decompressor = std::unique_ptr<libdeflate_decompressor, void (*)(libdeflate_decompressor*)>{
            libdeflate_alloc_decompressor(), libdeflate_free_decompressor
        };
        size_t actual_size = 0;
        libdeflate_result decompress_result = libdeflate_gzip_decompress(
            decompressor.get(), bytes, size, std::data(content), std::size(content), &actual_size);
        if (decompress_result == LIBDEFLATE_INSUFFICIENT_SPACE) {
            // need a bigger buffer
            content.resize(content.size() * 2);
            continue;
        }
        if (decompress_result == LIBDEFLATE_BAD_DATA) {
            THROW std::invalid_argument("bad gzip data in bb main");
        }
        content.resize(actual_size);
        break;
    }
    return content;
}

template <typename T> std::shared_ptr<T> read_to_shared_ptr(const std::filesystem::path& path)
{
    return std::make_shared<T>(from_buffer<T>(read_file(path)));
};

template <typename T> T unpack_from_file(const std::filesystem::path& filename)
{
    std::ifstream fin;
    fin.open(filename, std::ios::ate | std::ios::binary);
    if (!fin.is_open()) {
        THROW std::invalid_argument("file not found");
    }
    if (fin.tellg() == -1) {
        THROW std::invalid_argument("something went wrong");
    }

    size_t fsize = static_cast<size_t>(fin.tellg());
    fin.seekg(0, std::ios_base::beg);

    T result;
    char* encoded_data = new char[fsize];
    fin.read(encoded_data, static_cast<std::streamsize>(fsize));
    msgpack::unpack(encoded_data, fsize).get().convert(result);
    return result;
}

/**
 * @brief Compute and write to file a MegaHonk VK for a circuit to be accumulated in the IVC
 * @note This method differes from write_vk_honk<MegaFlavor> in that it handles kernel circuits which require special
 * treatment (i.e. construction of mock IVC state to correctly complete the kernel logic).
 *
 * @param bytecode_path
 * @param witness_path
 */
void write_standalone_vk(const std::string& output_data_type,
                         const std::string& bytecode_path,
                         const std::string& output_path)
{
    using Builder = ClientIVC::ClientCircuit;
    using Prover = ClientIVC::MegaProver;
    using DeciderProvingKey = ClientIVC::DeciderProvingKey;
    using VerificationKey = ClientIVC::MegaVerificationKey;
    using Program = acir_format::AcirProgram;
    using ProgramMetadata = acir_format::ProgramMetadata;
    using AggregationObject = stdlib::recursion::aggregation_state<Builder>;

    // TODO(https://github.com/AztecProtocol/barretenberg/issues/1163) set these dynamically
    init_bn254_crs(1 << CONST_PG_LOG_N);
    init_grumpkin_crs(1 << CONST_ECCVM_LOG_N);

    Program program{ get_constraint_system(bytecode_path, /*honk_recursion=*/0), /*witness=*/{} };
    auto& ivc_constraints = program.constraints.ivc_recursion_constraints;

    TraceSettings trace_settings{ AZTEC_TRACE_STRUCTURE };

    const ProgramMetadata metadata{ .ivc = ivc_constraints.empty()
                                               ? nullptr
                                               : create_mock_ivc_from_constraints(ivc_constraints, trace_settings) };
    Builder builder = acir_format::create_circuit<Builder>(program, metadata);

    // Add public inputs corresponding to pairing point accumulator
    AggregationObject::add_default_pairing_points_to_public_inputs(builder);

    // Construct the verification key via the prover-constructed proving key with the proper trace settings
    auto proving_key = std::make_shared<DeciderProvingKey>(builder, trace_settings);
    Prover prover{ proving_key };
    init_bn254_crs(prover.proving_key->proving_key.circuit_size);
    PubInputsProofAndKey<VerificationKey> to_write{
        PublicInputsVector{}, HonkProof{}, std::make_shared<VerificationKey>(prover.proving_key->proving_key)
    };

    write(to_write, output_data_type, "vk", output_path);
}

size_t get_num_public_inputs_in_final_circuit(const std::filesystem::path& bytecode_path)
{
    using namespace acir_format;

    const std::string bincode = unpack_from_file<std::vector<std::string>>(bytecode_path).back();
    const std::vector<uint8_t> bincode_buf = decompress(bincode.data(), bincode.size());
    const AcirFormat constraints = circuit_buf_to_acir_format(bincode_buf, /*honk_recursion=*/0);
    return constraints.public_inputs.size();
}

void write_vk_for_ivc(const std::string& bytecode_path, const std::filesystem::path& output_dir)
{
    init_bn254_crs(1 << CONST_PG_LOG_N);
    init_grumpkin_crs(1 << CONST_ECCVM_LOG_N);

    const size_t num_public_inputs_in_final_circuit = get_num_public_inputs_in_final_circuit(bytecode_path);
    info("num_public_inputs_in_final_circuit: ", num_public_inputs_in_final_circuit);
    // MAGIC_NUMBER is bb::PAIRING_POINT_ACCUMULATOR_SIZE or bb::PROPAGATED_DATABUS_COMMITMENTS_SIZE
    static constexpr size_t MAGIC_NUMBER = 16;

    ClientIVC ivc{ { AZTEC_TRACE_STRUCTURE } };
    ClientIVCMockCircuitProducer circuit_producer;

    // Initialize the IVC with an arbitrary circuit
    // We segfault if we only call accumulate once
    static constexpr size_t SMALL_ARBITRARY_LOG_CIRCUIT_SIZE{ 5 };
    MegaCircuitBuilder circuit_0 = circuit_producer.create_next_circuit(ivc, SMALL_ARBITRARY_LOG_CIRCUIT_SIZE);
    ivc.accumulate(circuit_0);

    // Create another circuit and accumulate
    MegaCircuitBuilder circuit_1 = circuit_producer.create_next_circuit(
        ivc, SMALL_ARBITRARY_LOG_CIRCUIT_SIZE, num_public_inputs_in_final_circuit + MAGIC_NUMBER);
    ivc.accumulate(circuit_1);

    // Construct the hiding circuit and its VK (stored internally in the IVC)
    ivc.construct_hiding_circuit_key();

    const bool output_to_stdout = output_dir == "-";
    const auto buf = to_buffer(ivc.get_vk());

    if (output_to_stdout) {
        write_bytes_to_stdout(buf);
    } else {
        write_file(output_dir / "vk", buf);
    }
}

std::vector<acir_format::AcirProgram> _build_folding_stack(const std::string& input_type,
                                                           const std::filesystem::path& bytecode_path,
                                                           const std::filesystem::path& witness_path)
{
    using namespace acir_format;

    std::vector<AcirProgram> folding_stack;

    // TODO(https://github.com/AztecProtocol/barretenberg/issues/1162): Efficiently unify ACIR stack parsing
    // really a single circuit IS a compiletime stack but we want the input type distinction since it is meaningful
    // for vk writing (maybe this is not ideal?)
    if (input_type == "single_circuit" || input_type == "compiletime_stack") {
        auto program_stack = acir_format::get_acir_program_stack(bytecode_path, witness_path, /*honk_recursion=*/0);
        // Accumulate the entire program stack into the IVC
        while (!program_stack.empty()) {
            auto stack_item = program_stack.back();
            folding_stack.push_back(AcirProgram{ stack_item.constraints, stack_item.witness });
            program_stack.pop_back();
        }
    }

    if (input_type == "runtime_stack") {
        std::vector<std::string> gzipped_bincodes = unpack_from_file<std::vector<std::string>>(bytecode_path);
        std::vector<std::string> witness_data = unpack_from_file<std::vector<std::string>>(witness_path);
        for (auto [bincode, wit] : zip_view(gzipped_bincodes, witness_data)) {
            // TODO(#7371) there is a lot of copying going on in bincode, we should make sure this writes as a
            // buffer in the future
            std::vector<uint8_t> constraint_buf = decompress(bincode.data(), bincode.size()); // NOLINT
            std::vector<uint8_t> witness_buf = decompress(wit.data(), wit.size());            // NOLINT

            AcirFormat constraints = circuit_buf_to_acir_format(constraint_buf, /*honk_recursion=*/0);
            WitnessVector witness = witness_buf_to_witness_data(witness_buf);

            folding_stack.push_back(AcirProgram{ constraints, witness });
        }
    }

    return folding_stack;
}

std::shared_ptr<ClientIVC> _accumulate(std::vector<acir_format::AcirProgram>& folding_stack)
{
    using Builder = MegaCircuitBuilder;
    using Program = acir_format::AcirProgram;
    using namespace acir_format;

    TraceSettings trace_settings{ AZTEC_TRACE_STRUCTURE };
    auto ivc = std::make_shared<ClientIVC>(trace_settings);

    const ProgramMetadata metadata{ ivc };

    // Accumulate the entire program stack into the IVC
    for (Program& program : folding_stack) {
        // Construct a bberg circuit from the acir representation then accumulate it into the IVC
        Builder circuit = acir_format::create_circuit<Builder>(program, metadata);

        // Do one step of ivc accumulator or, if there is only one circuit in the stack, prove that circuit. In this
        // case, no work is added to the Goblin opqueue, but VM proofs for trivials inputs are produced.
        ivc->accumulate(circuit);
    }

    return ivc;
}

void ClientIVCAPI::prove(const Flags& flags,
                         const std::filesystem::path& bytecode_path,
                         const std::filesystem::path& witness_path,
                         const std::filesystem::path& output_dir)
{
    // TODO(https://github.com/AztecProtocol/barretenberg/issues/1163) set these dynamically
    init_bn254_crs(1 << CONST_PG_LOG_N);
    init_grumpkin_crs(1 << CONST_ECCVM_LOG_N);

    std::vector<acir_format::AcirProgram> folding_stack =
        _build_folding_stack(flags.input_type, bytecode_path, witness_path);

    std::shared_ptr<ClientIVC> ivc = _accumulate(folding_stack);
    ClientIVC::Proof proof = ivc->prove();

    // We verify this proof. Another bb call to verify has the overhead of loading the SRS,
    // and it is mysterious if this transaction fails later in the lifecycle.
    // The files are still written in case they are needed to investigate this failure.
    if (!ivc->verify(proof)) {
        THROW std::runtime_error("Failed to verify the private (ClientIVC) transaction proof!");
    }

    // We'd like to use the `write` function that UltraHonkAPI uses, but there are missing functions for creating string
    // representations of vks that don't feel worth implementing
    const bool output_to_stdout = output_dir == "-";

    const auto write_proof = [&]() {
        const auto buf = to_buffer(proof);
        if (output_to_stdout) {
            vinfo("writing ClientIVC proof to stdout");
            write_bytes_to_stdout(buf);
        } else {
            vinfo("writing ClientIVC proof in directory ", output_dir);
            proof.to_file_msgpack(output_dir / "proof");
        }
    };

    write_proof();

    if (flags.write_vk) {
        vinfo("writing ClientIVC vk in directory ", output_dir);
        write_vk_for_ivc(bytecode_path, output_dir);
    }
}

bool ClientIVCAPI::verify([[maybe_unused]] const Flags& flags,
                          [[maybe_unused]] const std::filesystem::path& public_inputs_path,
                          const std::filesystem::path& proof_path,
                          const std::filesystem::path& vk_path)
{
    // TODO(https://github.com/AztecProtocol/barretenberg/issues/1163): Set these dynamically
    init_bn254_crs(1);
    init_grumpkin_crs(1 << CONST_ECCVM_LOG_N);

    const auto proof = ClientIVC::Proof::from_file_msgpack(proof_path);
    const auto vk = from_buffer<ClientIVC::VerificationKey>(read_file(vk_path));

    // TODO(https://github.com/AztecProtocol/barretenberg/issues/1335): Should be able to remove this.
    vk.mega->pcs_verification_key = std::make_shared<VerifierCommitmentKey<curve::BN254>>();

    const bool verified = ClientIVC::verify(proof, vk);
    return verified;
}

bool ClientIVCAPI::prove_and_verify(const Flags& flags,
                                    const std::filesystem::path& bytecode_path,
                                    const std::filesystem::path& witness_path)
{
    if (!(flags.input_type == "compiletime_stack" || flags.input_type == "runtime_stack")) {
        throw_or_abort("No input_type or input_type not supported");
    }

    // TODO(https://github.com/AztecProtocol/barretenberg/issues/1163) set these dynamically
    init_bn254_crs(1 << CONST_PG_LOG_N);
    init_grumpkin_crs(1 << CONST_ECCVM_LOG_N);

    std::vector<acir_format::AcirProgram> folding_stack =
        _build_folding_stack(flags.input_type, bytecode_path, witness_path);
    std::shared_ptr<ClientIVC> ivc = _accumulate(folding_stack);
    const bool verified = ivc->prove_and_verify();
    return verified;
}

void ClientIVCAPI::gates([[maybe_unused]] const Flags& flags,
                         [[maybe_unused]] const std::filesystem::path& bytecode_path)
{
    gate_count_for_ivc(bytecode_path, flags.include_gates_per_opcode);
}

void ClientIVCAPI::write_solidity_verifier([[maybe_unused]] const Flags& flags,
                                           [[maybe_unused]] const std::filesystem::path& output_path,
                                           [[maybe_unused]] const std::filesystem::path& vk_path)
{
    throw_or_abort("API function contract not implemented");
}

void ClientIVCAPI::write_vk(const Flags& flags,
                            const std::filesystem::path& bytecode_path,
                            const std::filesystem::path& output_path)
{
    if (flags.verifier_type == "ivc") {
        write_vk_for_ivc(bytecode_path, output_path);
    } else if (flags.verifier_type == "standalone") {
        write_standalone_vk(flags.output_format, bytecode_path, output_path);
    } else {
        const std::string msg = std::string("Can't write vk for verifier type ") + flags.verifier_type;
        throw_or_abort(msg);
    }
}

bool ClientIVCAPI::check([[maybe_unused]] const Flags& flags,
                         [[maybe_unused]] const std::filesystem::path& bytecode_path,
                         [[maybe_unused]] const std::filesystem::path& witness_path)
{
    throw_or_abort("API function check_witness not implemented");
    return false;
}

void gate_count_for_ivc(const std::string& bytecode_path, bool include_gates_per_opcode)
{
    // All circuit reports will be built into the string below
    std::string functions_string = "{\"functions\": [\n  ";
    // TODO(https://github.com/AztecProtocol/barretenberg/issues/1181): Use enum for honk_recursion.
    auto constraint_systems = get_constraint_systems(bytecode_path, /*honk_recursion=*/0);

    // Initialize an SRS to make the ClientIVC constructor happy
    init_bn254_crs(1 << CONST_PG_LOG_N);
    init_grumpkin_crs(1 << CONST_ECCVM_LOG_N);
    TraceSettings trace_settings{ AZTEC_TRACE_STRUCTURE };

    size_t i = 0;
    for (const auto& constraint_system : constraint_systems) {
        acir_format::AcirProgram program{ constraint_system };
        const auto& ivc_constraints = constraint_system.ivc_recursion_constraints;
        acir_format::ProgramMetadata metadata{ .ivc = ivc_constraints.empty() ? nullptr
                                                                              : create_mock_ivc_from_constraints(
                                                                                    ivc_constraints, trace_settings),
                                               .collect_gates_per_opcode = include_gates_per_opcode };

        auto builder = acir_format::create_circuit<MegaCircuitBuilder>(program, metadata);
        builder.finalize_circuit(/*ensure_nonzero=*/true);
        size_t circuit_size = builder.num_gates;

        // Print the details of the gate types within the structured execution trace
        builder.blocks.set_fixed_block_sizes(trace_settings);
        builder.blocks.summarize();

        // Build individual circuit report
        std::string gates_per_opcode_str;
        for (size_t j = 0; j < program.constraints.gates_per_opcode.size(); j++) {
            gates_per_opcode_str += std::to_string(program.constraints.gates_per_opcode[j]);
            if (j != program.constraints.gates_per_opcode.size() - 1) {
                gates_per_opcode_str += ",";
            }
        }

        auto result_string = format(
            "{\n        \"acir_opcodes\": ",
            program.constraints.num_acir_opcodes,
            ",\n        \"circuit_size\": ",
            circuit_size,
            (include_gates_per_opcode ? format(",\n        \"gates_per_opcode\": [", gates_per_opcode_str, "]") : ""),
            "\n  }");

        // Attach a comma if there are more circuit reports to generate
        if (i != (constraint_systems.size() - 1)) {
            result_string = format(result_string, ",");
        }

        functions_string = format(functions_string, result_string);

        i++;
    }
    std::cout << format(functions_string, "\n]}");
}

void write_arbitrary_valid_client_ivc_proof_and_vk_to_file(const std::filesystem::path& output_dir)
{
    // TODO(https://github.com/AztecProtocol/barretenberg/issues/1163) set these dynamically
    init_bn254_crs(1 << CONST_PG_LOG_N);
    init_grumpkin_crs(1 << CONST_ECCVM_LOG_N);

    ClientIVC ivc{ { CLIENT_IVC_BENCH_STRUCTURE } };

    // Construct and accumulate a series of mocked private function execution circuits
    PrivateFunctionExecutionMockCircuitProducer circuit_producer;
    size_t NUM_CIRCUITS = 2;
    for (size_t idx = 0; idx < NUM_CIRCUITS; ++idx) {
        auto circuit = circuit_producer.create_next_circuit(ivc);
        ivc.accumulate(circuit);
    }

    ClientIVC::Proof proof = ivc.prove();

    // Write the proof and verification keys into the working directory in 'binary' format
    vinfo("writing ClientIVC proof and vk...");
    proof.to_file_msgpack(output_dir / "proof");

    write_file(output_dir / "vk", to_buffer(ivc.get_vk()));
}

} // namespace bb
