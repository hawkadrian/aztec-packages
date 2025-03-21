// AUTOGENERATED FILE
#pragma once

#include "../columns.hpp"
#include "barretenberg/relations/generic_lookup/generic_lookup_relation.hpp"

#include <cstddef>
#include <string_view>
#include <tuple>

namespace bb::avm {

/////////////////// lookup_rng_chk_pow_2 ///////////////////

class lookup_rng_chk_pow_2_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_POW_2";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 2;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_rng_chk;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_8;
    static constexpr Column COUNTS = Column::lookup_rng_chk_pow_2_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_pow_2_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_dyn_rng_chk_bits,
                                                                           Column::range_check_dyn_rng_chk_pow_2 };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk,
                                                                           Column::powers_power_of_2 };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_rng_chk() == 1 || in._main_sel_rng_8() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_rng_chk());
        const auto is_table_entry = View(in._main_sel_rng_8());
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
        return std::forward_as_tuple(in._lookup_rng_chk_pow_2_inv(),
                                     in._lookup_rng_chk_pow_2_counts(),
                                     in._range_check_sel_rng_chk(),
                                     in._main_sel_rng_8(),
                                     in._range_check_dyn_rng_chk_bits(),
                                     in._range_check_dyn_rng_chk_pow_2(),
                                     in._main_clk(),
                                     in._powers_power_of_2());
    }
};

template <typename FF_>
class lookup_rng_chk_pow_2_relation : public GenericLookupRelation<lookup_rng_chk_pow_2_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_pow_2_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_rng_chk.is_zero() && in.main_sel_rng_8.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_pow_2 = GenericLookup<lookup_rng_chk_pow_2_lookup_settings, FF_>;

/////////////////// lookup_rng_chk_diff ///////////////////

class lookup_rng_chk_diff_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_DIFF";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_rng_chk;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_16;
    static constexpr Column COUNTS = Column::lookup_rng_chk_diff_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_diff_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_dyn_diff };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_rng_chk() == 1 || in._main_sel_rng_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_rng_chk());
        const auto is_table_entry = View(in._main_sel_rng_16());
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
        return std::forward_as_tuple(in._lookup_rng_chk_diff_inv(),
                                     in._lookup_rng_chk_diff_counts(),
                                     in._range_check_sel_rng_chk(),
                                     in._main_sel_rng_16(),
                                     in._range_check_dyn_diff(),
                                     in._main_clk());
    }
};

template <typename FF_>
class lookup_rng_chk_diff_relation : public GenericLookupRelation<lookup_rng_chk_diff_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_diff_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_rng_chk.is_zero() && in.main_sel_rng_16.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_diff = GenericLookup<lookup_rng_chk_diff_lookup_settings, FF_>;

/////////////////// lookup_rng_chk_0 ///////////////////

class lookup_rng_chk_0_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_0";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_lookup_0;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_16;
    static constexpr Column COUNTS = Column::lookup_rng_chk_0_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_0_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_u16_r0 };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_lookup_0() == 1 || in._main_sel_rng_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_lookup_0());
        const auto is_table_entry = View(in._main_sel_rng_16());
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
        return std::forward_as_tuple(in._lookup_rng_chk_0_inv(),
                                     in._lookup_rng_chk_0_counts(),
                                     in._range_check_sel_lookup_0(),
                                     in._main_sel_rng_16(),
                                     in._range_check_u16_r0(),
                                     in._main_clk());
    }
};

template <typename FF_>
class lookup_rng_chk_0_relation : public GenericLookupRelation<lookup_rng_chk_0_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_0_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_lookup_0.is_zero() && in.main_sel_rng_16.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_0 = GenericLookup<lookup_rng_chk_0_lookup_settings, FF_>;

/////////////////// lookup_rng_chk_1 ///////////////////

class lookup_rng_chk_1_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_1";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_lookup_1;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_16;
    static constexpr Column COUNTS = Column::lookup_rng_chk_1_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_1_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_u16_r1 };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_lookup_1() == 1 || in._main_sel_rng_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_lookup_1());
        const auto is_table_entry = View(in._main_sel_rng_16());
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
        return std::forward_as_tuple(in._lookup_rng_chk_1_inv(),
                                     in._lookup_rng_chk_1_counts(),
                                     in._range_check_sel_lookup_1(),
                                     in._main_sel_rng_16(),
                                     in._range_check_u16_r1(),
                                     in._main_clk());
    }
};

template <typename FF_>
class lookup_rng_chk_1_relation : public GenericLookupRelation<lookup_rng_chk_1_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_1_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_lookup_1.is_zero() && in.main_sel_rng_16.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_1 = GenericLookup<lookup_rng_chk_1_lookup_settings, FF_>;

/////////////////// lookup_rng_chk_2 ///////////////////

class lookup_rng_chk_2_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_2";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_lookup_2;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_16;
    static constexpr Column COUNTS = Column::lookup_rng_chk_2_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_2_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_u16_r2 };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_lookup_2() == 1 || in._main_sel_rng_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_lookup_2());
        const auto is_table_entry = View(in._main_sel_rng_16());
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
        return std::forward_as_tuple(in._lookup_rng_chk_2_inv(),
                                     in._lookup_rng_chk_2_counts(),
                                     in._range_check_sel_lookup_2(),
                                     in._main_sel_rng_16(),
                                     in._range_check_u16_r2(),
                                     in._main_clk());
    }
};

template <typename FF_>
class lookup_rng_chk_2_relation : public GenericLookupRelation<lookup_rng_chk_2_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_2_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_lookup_2.is_zero() && in.main_sel_rng_16.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_2 = GenericLookup<lookup_rng_chk_2_lookup_settings, FF_>;

/////////////////// lookup_rng_chk_3 ///////////////////

class lookup_rng_chk_3_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_3";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_lookup_3;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_16;
    static constexpr Column COUNTS = Column::lookup_rng_chk_3_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_3_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_u16_r3 };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_lookup_3() == 1 || in._main_sel_rng_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_lookup_3());
        const auto is_table_entry = View(in._main_sel_rng_16());
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
        return std::forward_as_tuple(in._lookup_rng_chk_3_inv(),
                                     in._lookup_rng_chk_3_counts(),
                                     in._range_check_sel_lookup_3(),
                                     in._main_sel_rng_16(),
                                     in._range_check_u16_r3(),
                                     in._main_clk());
    }
};

template <typename FF_>
class lookup_rng_chk_3_relation : public GenericLookupRelation<lookup_rng_chk_3_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_3_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_lookup_3.is_zero() && in.main_sel_rng_16.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_3 = GenericLookup<lookup_rng_chk_3_lookup_settings, FF_>;

/////////////////// lookup_rng_chk_4 ///////////////////

class lookup_rng_chk_4_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_4";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_lookup_4;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_16;
    static constexpr Column COUNTS = Column::lookup_rng_chk_4_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_4_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_u16_r4 };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_lookup_4() == 1 || in._main_sel_rng_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_lookup_4());
        const auto is_table_entry = View(in._main_sel_rng_16());
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
        return std::forward_as_tuple(in._lookup_rng_chk_4_inv(),
                                     in._lookup_rng_chk_4_counts(),
                                     in._range_check_sel_lookup_4(),
                                     in._main_sel_rng_16(),
                                     in._range_check_u16_r4(),
                                     in._main_clk());
    }
};

template <typename FF_>
class lookup_rng_chk_4_relation : public GenericLookupRelation<lookup_rng_chk_4_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_4_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_lookup_4.is_zero() && in.main_sel_rng_16.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_4 = GenericLookup<lookup_rng_chk_4_lookup_settings, FF_>;

/////////////////// lookup_rng_chk_5 ///////////////////

class lookup_rng_chk_5_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_5";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_lookup_5;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_16;
    static constexpr Column COUNTS = Column::lookup_rng_chk_5_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_5_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_u16_r5 };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_lookup_5() == 1 || in._main_sel_rng_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_lookup_5());
        const auto is_table_entry = View(in._main_sel_rng_16());
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
        return std::forward_as_tuple(in._lookup_rng_chk_5_inv(),
                                     in._lookup_rng_chk_5_counts(),
                                     in._range_check_sel_lookup_5(),
                                     in._main_sel_rng_16(),
                                     in._range_check_u16_r5(),
                                     in._main_clk());
    }
};

template <typename FF_>
class lookup_rng_chk_5_relation : public GenericLookupRelation<lookup_rng_chk_5_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_5_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_lookup_5.is_zero() && in.main_sel_rng_16.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_5 = GenericLookup<lookup_rng_chk_5_lookup_settings, FF_>;

/////////////////// lookup_rng_chk_6 ///////////////////

class lookup_rng_chk_6_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_6";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_lookup_6;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_16;
    static constexpr Column COUNTS = Column::lookup_rng_chk_6_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_6_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_u16_r6 };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_lookup_6() == 1 || in._main_sel_rng_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_lookup_6());
        const auto is_table_entry = View(in._main_sel_rng_16());
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
        return std::forward_as_tuple(in._lookup_rng_chk_6_inv(),
                                     in._lookup_rng_chk_6_counts(),
                                     in._range_check_sel_lookup_6(),
                                     in._main_sel_rng_16(),
                                     in._range_check_u16_r6(),
                                     in._main_clk());
    }
};

template <typename FF_>
class lookup_rng_chk_6_relation : public GenericLookupRelation<lookup_rng_chk_6_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_6_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_lookup_6.is_zero() && in.main_sel_rng_16.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_6 = GenericLookup<lookup_rng_chk_6_lookup_settings, FF_>;

/////////////////// lookup_rng_chk_7 ///////////////////

class lookup_rng_chk_7_lookup_settings {
  public:
    static constexpr std::string_view NAME = "LOOKUP_RNG_CHK_7";

    static constexpr size_t READ_TERMS = 1;
    static constexpr size_t WRITE_TERMS = 1;
    static constexpr size_t READ_TERM_TYPES[READ_TERMS] = { 0 };
    static constexpr size_t WRITE_TERM_TYPES[WRITE_TERMS] = { 0 };
    static constexpr size_t LOOKUP_TUPLE_SIZE = 1;
    static constexpr size_t INVERSE_EXISTS_POLYNOMIAL_DEGREE = 4;
    static constexpr size_t READ_TERM_DEGREE = 0;
    static constexpr size_t WRITE_TERM_DEGREE = 0;

    // Columns using the Column enum.
    static constexpr Column SRC_SELECTOR = Column::range_check_sel_rng_chk;
    static constexpr Column DST_SELECTOR = Column::main_sel_rng_16;
    static constexpr Column COUNTS = Column::lookup_rng_chk_7_counts;
    static constexpr Column INVERSES = Column::lookup_rng_chk_7_inv;
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> SRC_COLUMNS = { Column::range_check_u16_r7 };
    static constexpr std::array<Column, LOOKUP_TUPLE_SIZE> DST_COLUMNS = { Column::main_clk };

    template <typename AllEntities> static inline auto inverse_polynomial_is_computed_at_row(const AllEntities& in)
    {
        return (in._range_check_sel_rng_chk() == 1 || in._main_sel_rng_16() == 1);
    }

    template <typename Accumulator, typename AllEntities>
    static inline auto compute_inverse_exists(const AllEntities& in)
    {
        using View = typename Accumulator::View;
        const auto is_operation = View(in._range_check_sel_rng_chk());
        const auto is_table_entry = View(in._main_sel_rng_16());
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
        return std::forward_as_tuple(in._lookup_rng_chk_7_inv(),
                                     in._lookup_rng_chk_7_counts(),
                                     in._range_check_sel_rng_chk(),
                                     in._main_sel_rng_16(),
                                     in._range_check_u16_r7(),
                                     in._main_clk());
    }
};

template <typename FF_>
class lookup_rng_chk_7_relation : public GenericLookupRelation<lookup_rng_chk_7_lookup_settings, FF_> {
  public:
    static constexpr std::string_view NAME = lookup_rng_chk_7_lookup_settings::NAME;

    template <typename AllEntities> inline static bool skip(const AllEntities& in)
    {
        return in.range_check_sel_rng_chk.is_zero() && in.main_sel_rng_16.is_zero();
    }
};
template <typename FF_> using lookup_rng_chk_7 = GenericLookup<lookup_rng_chk_7_lookup_settings, FF_>;

} // namespace bb::avm