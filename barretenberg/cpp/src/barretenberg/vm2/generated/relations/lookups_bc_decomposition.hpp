// AUTOGENERATED FILE
#pragma once

#include "../columns.hpp"
#include "barretenberg/relations/generic_lookup/generic_lookup_relation.hpp"

#include <cstddef>
#include <string_view>
#include <tuple>

namespace bb::avm2 {

/////////////////// lookup_bc_decomposition_bytes_are_bytes ///////////////////

class lookup_bc_decomposition_bytes_are_bytes_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_BC_DECOMPOSITION_BYTES_ARE_BYTES";
    static constexpr std::string_view RELATION_NAME = "bc_decomposition";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::bc_decomposition_sel;
    static constexpr Column DST_SELECTOR = Column::precomputed_sel_range_8;
    static constexpr Column COUNTS = Column::lookup_bc_decomposition_bytes_are_bytes_counts;
    static constexpr Column INVERSES = Column::lookup_bc_decomposition_bytes_are_bytes_inv;
    static constexpr std::array<ColumnAndShifts, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = {
        ColumnAndShifts::bc_decomposition_bytes
    };
    static constexpr std::array<ColumnAndShifts, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { ColumnAndShifts::precomputed_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._bc_decomposition_sel() == 1 || in._precomputed_sel_range_8() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._bc_decomposition_sel());
        const auto is_table_entry = View(in._precomputed_sel_range_8());
        return (is_operation + is_table_entry - is_operation * is_table_entry);
    }

    template <typename AllEntities> static inline auto get_const_entities(const AllEntities& in)
    {
        return get_entities(in);
    }

    template <typename AllEntities> static inline auto get_nonconst_entities(AllEntities& in)
    {
        return get_entities(in);
    }

    template <typename AllEntities> static inline auto get_entities(AllEntities&& in)
    {
        return std::forward_as_tuple(in._lookup_bc_decomposition_bytes_are_bytes_inv(),
                                     in._lookup_bc_decomposition_bytes_are_bytes_counts(),
                                     in._bc_decomposition_sel(),
                                     in._precomputed_sel_range_8(),
                                     in._bc_decomposition_bytes(),
                                     in._precomputed_clk());
    }
};

template <typename FF_>
class lookup_bc_decomposition_bytes_are_bytes_relation
    : public GenericLookupRelation<lookup_bc_decomposition_bytes_are_bytes_settings, FF_> {
  public:
    using Settings = lookup_bc_decomposition_bytes_are_bytes_settings;
    static constexpr std::string_view NAME = lookup_bc_decomposition_bytes_are_bytes_settings::NAME;
    static constexpr std::string_view RELATION_NAME = lookup_bc_decomposition_bytes_are_bytes_settings::RELATION_NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.lookup_bc_decomposition_bytes_are_bytes_inv.is_zero();
    }

    static std::string get_subrelation_label(size_t index)
    {
        if (index == 0) {
            return "INVERSES_ARE_CORRECT";
        } else if (index == 1) {
            return "ACCUMULATION_IS_CORRECT";
        }
        return std::to_string(index);
    }
};

/////////////////// lookup_bc_decomposition_abs_diff_is_u16 ///////////////////

class lookup_bc_decomposition_abs_diff_is_u16_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_BC_DECOMPOSITION_ABS_DIFF_IS_U16";
    static constexpr std::string_view RELATION_NAME = "bc_decomposition";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::bc_decomposition_sel;
    static constexpr Column DST_SELECTOR = Column::precomputed_sel_range_16;
    static constexpr Column COUNTS = Column::lookup_bc_decomposition_abs_diff_is_u16_counts;
    static constexpr Column INVERSES = Column::lookup_bc_decomposition_abs_diff_is_u16_inv;
    static constexpr std::array<ColumnAndShifts, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = {
        ColumnAndShifts::bc_decomposition_abs_diff
    };
    static constexpr std::array<ColumnAndShifts, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { ColumnAndShifts::precomputed_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._bc_decomposition_sel() == 1 || in._precomputed_sel_range_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._bc_decomposition_sel());
        const auto is_table_entry = View(in._precomputed_sel_range_16());
        return (is_operation + is_table_entry - is_operation * is_table_entry);
    }

    template <typename AllEntities> static inline auto get_const_entities(const AllEntities& in)
    {
        return get_entities(in);
    }

    template <typename AllEntities> static inline auto get_nonconst_entities(AllEntities& in)
    {
        return get_entities(in);
    }

    template <typename AllEntities> static inline auto get_entities(AllEntities&& in)
    {
        return std::forward_as_tuple(in._lookup_bc_decomposition_abs_diff_is_u16_inv(),
                                     in._lookup_bc_decomposition_abs_diff_is_u16_counts(),
                                     in._bc_decomposition_sel(),
                                     in._precomputed_sel_range_16(),
                                     in._bc_decomposition_abs_diff(),
                                     in._precomputed_clk());
    }
};

template <typename FF_>
class lookup_bc_decomposition_abs_diff_is_u16_relation
    : public GenericLookupRelation<lookup_bc_decomposition_abs_diff_is_u16_settings, FF_> {
  public:
    using Settings = lookup_bc_decomposition_abs_diff_is_u16_settings;
    static constexpr std::string_view NAME = lookup_bc_decomposition_abs_diff_is_u16_settings::NAME;
    static constexpr std::string_view RELATION_NAME = lookup_bc_decomposition_abs_diff_is_u16_settings::RELATION_NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.lookup_bc_decomposition_abs_diff_is_u16_inv.is_zero();
    }

    static std::string get_subrelation_label(size_t index)
    {
        if (index == 0) {
            return "INVERSES_ARE_CORRECT";
        } else if (index == 1) {
            return "ACCUMULATION_IS_CORRECT";
        }
        return std::to_string(index);
    }
};

} // namespace bb::avm2
