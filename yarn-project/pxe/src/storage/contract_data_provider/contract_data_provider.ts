import type { Fr } from '@aztec/foundation/fields';
import { toArray } from '@aztec/foundation/iterable';
import type { MembershipWitness } from '@aztec/foundation/trees';
import type { AztecAsyncKVStore, AztecAsyncMap } from '@aztec/kv-store';
import { ContractClassNotFoundError, ContractNotFoundError } from '@aztec/simulator/client';
import {
  type ContractArtifact,
  type FunctionArtifact,
  type FunctionArtifactWithContractName,
  type FunctionDebugMetadata,
  FunctionSelector,
  FunctionType,
  contractArtifactFromBuffer,
  contractArtifactToBuffer,
  getFunctionDebugMetadata,
} from '@aztec/stdlib/abi';
import { AztecAddress } from '@aztec/stdlib/aztec-address';
import {
  type ContractClass,
  type ContractInstanceWithAddress,
  SerializableContractInstance,
} from '@aztec/stdlib/contract';

import type { DataProvider } from '../data_provider.js';
import { PrivateFunctionsTree } from './private_functions_tree.js';

/**
 * ContractDataProvider serves as a data manager and retriever for Aztec.nr contracts.
 * It provides methods to obtain contract addresses, function ABI, bytecode, and membership witnesses
 * from a given contract address and function selector. The class maintains a cache of ContractTree instances
 * to efficiently serve the requested data. It interacts with the ContractDatabase and AztecNode to fetch
 * the required information and facilitate cryptographic proof generation.
 */
export class ContractDataProvider implements DataProvider {
  /** Map from contract class id to private function tree. */
  private contractClassesCache: Map<string, PrivateFunctionsTree> = new Map();

  #contractArtifacts: AztecAsyncMap<string, Buffer>;
  #contractInstances: AztecAsyncMap<string, Buffer>;

  constructor(store: AztecAsyncKVStore) {
    this.#contractArtifacts = store.openMap('contract_artifacts');
    this.#contractInstances = store.openMap('contracts_instances');
  }

  // Setters

  public async addContractArtifact(id: Fr, contract: ContractArtifact): Promise<void> {
    const privateFunctions = contract.functions.filter(
      functionArtifact => functionArtifact.functionType === FunctionType.PRIVATE,
    );

    const privateSelectors = await Promise.all(
      privateFunctions.map(async privateFunctionArtifact =>
        (
          await FunctionSelector.fromNameAndParameters(privateFunctionArtifact.name, privateFunctionArtifact.parameters)
        ).toString(),
      ),
    );

    if (privateSelectors.length !== new Set(privateSelectors).size) {
      throw new Error('Repeated function selectors of private functions');
    }

    await this.#contractArtifacts.set(id.toString(), contractArtifactToBuffer(contract));
  }

  async addContractInstance(contract: ContractInstanceWithAddress): Promise<void> {
    await this.#contractInstances.set(
      contract.address.toString(),
      new SerializableContractInstance(contract).toBuffer(),
    );
  }

  // Private getters

  async #getContractInstance(address: AztecAddress): Promise<ContractInstanceWithAddress | undefined> {
    const contract = await this.#contractInstances.getAsync(address.toString());
    return contract && SerializableContractInstance.fromBuffer(contract).withAddress(address);
  }

  async #getContractArtifact(id: Fr): Promise<ContractArtifact | undefined> {
    const contract = await this.#contractArtifacts.getAsync(id.toString());
    // TODO(@spalladino): AztecAsyncMap lies and returns Uint8Arrays instead of Buffers, hence the extra Buffer.from.
    return contract && contractArtifactFromBuffer(Buffer.from(contract));
  }

  /**
   * Retrieve or create a ContractTree instance based on the provided class id.
   * If an existing tree with the same class id is found in the cache, it will be returned.
   * Otherwise, a new ContractTree instance will be created using the contract data from the database
   * and added to the cache before returning.
   *
   * @param classId - The class id of the contract for which the ContractTree is required.
   * @returns A ContractTree instance associated with the specified contract address.
   * @throws An Error if the contract is not found in the ContractDatabase.
   */
  private async getTreeForClassId(classId: Fr): Promise<PrivateFunctionsTree> {
    if (!this.contractClassesCache.has(classId.toString())) {
      const artifact = await this.#getContractArtifact(classId);
      if (!artifact) {
        throw new ContractClassNotFoundError(classId.toString());
      }
      const tree = await PrivateFunctionsTree.create(artifact);
      this.contractClassesCache.set(classId.toString(), tree);
    }
    return this.contractClassesCache.get(classId.toString())!;
  }

  /**
   * Retrieve or create a ContractTree instance based on the provided AztecAddress.
   * If an existing tree with the same contract address is found in the cache, it will be returned.
   * Otherwise, a new ContractTree instance will be created using the contract data from the database
   * and added to the cache before returning.
   *
   * @param contractAddress - The AztecAddress of the contract for which the ContractTree is required.
   * @returns A ContractTree instance associated with the specified contract address.
   * @throws An Error if the contract is not found in the ContractDatabase.
   */
  private async getTreeForAddress(contractAddress: AztecAddress): Promise<PrivateFunctionsTree> {
    const instance = await this.getContractInstance(contractAddress);
    return this.getTreeForClassId(instance.currentContractClassId);
  }

  // Public getters

  async getContractsAddresses(): Promise<AztecAddress[]> {
    const keys = await toArray(this.#contractInstances.keysAsync());
    return keys.map(AztecAddress.fromString);
  }

  /** Returns a contract instance for a given address. Throws if not found. */
  public async getContractInstance(contractAddress: AztecAddress): Promise<ContractInstanceWithAddress> {
    const instance = await this.#getContractInstance(contractAddress);
    if (!instance) {
      throw new ContractNotFoundError(contractAddress.toString());
    }
    return instance;
  }

  public async getContractArtifact(contractClassId: Fr): Promise<ContractArtifact> {
    const tree = await this.getTreeForClassId(contractClassId);
    return tree.getArtifact();
  }

  /** Returns a contract class for a given class id. Throws if not found. */
  public async getContractClass(contractClassId: Fr): Promise<ContractClass> {
    const tree = await this.getTreeForClassId(contractClassId);
    return tree.getContractClass();
  }

  public async getContract(
    address: AztecAddress,
  ): Promise<(ContractInstanceWithAddress & ContractArtifact) | undefined> {
    const instance = await this.getContractInstance(address);
    const artifact = instance && (await this.getContractArtifact(instance?.currentContractClassId));
    if (!instance || !artifact) {
      return undefined;
    }
    return { ...instance, ...artifact };
  }

  /**
   * Retrieves the artifact of a specified function within a given contract.
   * The function is identified by its selector, which is a unique code generated from the function's signature.
   * Throws an error if the contract address or function selector are invalid or not found.
   *
   * @param contractAddress - The AztecAddress representing the contract containing the function.
   * @param selector - The function selector.
   * @returns The corresponding function's artifact as an object.
   */
  public async getFunctionArtifact(
    contractAddress: AztecAddress,
    selector: FunctionSelector,
  ): Promise<FunctionArtifactWithContractName> {
    const tree = await this.getTreeForAddress(contractAddress);
    const contractArtifact = tree.getArtifact();
    const functionArtifact = await tree.getFunctionArtifact(selector);
    return {
      ...functionArtifact,
      contractName: contractArtifact.name,
    };
  }

  /**
   * Retrieves the artifact of a specified function within a given contract.
   * The function is identified by its name, which is unique within a contract.
   * Throws if the contract has not been added to the database.
   *
   * @param contractAddress - The AztecAddress representing the contract containing the function.
   * @param functionName - The name of the function.
   * @returns The corresponding function's artifact as an object
   */
  public async getFunctionArtifactByName(
    contractAddress: AztecAddress,
    functionName: string,
  ): Promise<FunctionArtifact | undefined> {
    const tree = await this.getTreeForAddress(contractAddress);
    return tree.getArtifact().functions.find(f => f.name === functionName);
  }

  /**
   * Retrieves the debug metadata of a specified function within a given contract.
   * The function is identified by its selector, which is a unique code generated from the function's signature.
   * Returns undefined if the debug metadata for the given function is not found.
   * Throws if the contract has not been added to the database.
   *
   * @param contractAddress - The AztecAddress representing the contract containing the function.
   * @param selector - The function selector.
   * @returns The corresponding function's artifact as an object.
   */
  public async getFunctionDebugMetadata(
    contractAddress: AztecAddress,
    selector: FunctionSelector,
  ): Promise<FunctionDebugMetadata | undefined> {
    const tree = await this.getTreeForAddress(contractAddress);
    const artifact = await tree.getFunctionArtifact(selector);
    return getFunctionDebugMetadata(tree.getArtifact(), artifact);
  }

  /**
   * Retrieve the bytecode of a specific function in a contract at the given address.
   * The returned bytecode is required for executing and verifying the function's behavior
   * in the Aztec network. Throws an error if the contract or function cannot be found.
   *
   * @param contractAddress - The contract's address.
   * @param selector - The function selector.
   * @returns A Promise that resolves to a Buffer containing the bytecode of the specified function.
   * @throws Error if the contract address is unknown or not found.
   */
  public async getBytecode(contractAddress: AztecAddress, selector: FunctionSelector) {
    const tree = await this.getTreeForAddress(contractAddress);
    return tree.getBytecode(selector);
  }

  /**
   * Retrieve the function membership witness for the given contract class and function selector.
   * The function membership witness represents a proof that the function belongs to the specified contract.
   * Throws an error if the contract address or function selector is unknown.
   *
   * @param contractClassId - The id of the class.
   * @param selector - The function selector.
   * @returns A promise that resolves with the MembershipWitness instance for the specified contract's function.
   */
  public async getFunctionMembershipWitness(
    contractClassId: Fr,
    selector: FunctionSelector,
  ): Promise<MembershipWitness<5>> {
    const tree = await this.getTreeForClassId(contractClassId);
    return tree.getFunctionMembershipWitness(selector);
  }

  public async getDebugContractName(contractAddress: AztecAddress) {
    const tree = await this.getTreeForAddress(contractAddress);
    return tree.getArtifact().name;
  }

  public async getDebugFunctionName(contractAddress: AztecAddress, selector: FunctionSelector) {
    const tree = await this.getTreeForAddress(contractAddress);
    const { name: contractName } = tree.getArtifact();
    const { name: functionName } = await tree.getFunctionArtifact(selector);
    return `${contractName}:${functionName}`;
  }

  public async getSize() {
    return (await toArray(this.#contractInstances.valuesAsync()))
      .concat(await toArray(this.#contractArtifacts.valuesAsync()))
      .reduce((sum, value) => sum + value.length, 0);
  }
}
