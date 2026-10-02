#include "test_common.hpp"

#include <proc_macro.hpp>

int main()
{
    auto implicit_defaults = proc_macro::RecordWithImplicitDefaults{};

    ASSERT_EQ(false, implicit_defaults.boolean);
    ASSERT_EQ(0, implicit_defaults.int8);
    ASSERT_EQ(0, implicit_defaults.uint8);
    ASSERT_EQ(0, implicit_defaults.int16);
    ASSERT_EQ(0, implicit_defaults.uint16);
    ASSERT_EQ(0, implicit_defaults.int32);
    ASSERT_EQ(0, implicit_defaults.uint32);
    ASSERT_EQ(0, implicit_defaults.int64);
    ASSERT_EQ(0, implicit_defaults.uint64);
    ASSERT_EQ(0.0f, implicit_defaults.afloat);
    ASSERT_EQ(0.0, implicit_defaults.adouble);
    ASSERT_EQ(std::nullopt, implicit_defaults.opt_int32);
    ASSERT_TRUE(implicit_defaults.vec.empty());
    ASSERT_TRUE(implicit_defaults.map.empty());
    ASSERT_TRUE(implicit_defaults.some_bytes.empty());

    ASSERT_NE(nullptr, implicit_defaults.object);
    ASSERT_EQ(proc_macro::MaybeBool::kUncertain, implicit_defaults.object->is_heavy());
    ASSERT_EQ(0, implicit_defaults.custom_integer);

    auto error = std::make_shared<proc_macro::basic_error::InvalidInput>();
    ASSERT_EQ(42, implicit_defaults.object->take_error(error));

    auto defaults = proc_macro::RecordWithDefaults{};

    ASSERT_EQ(true, defaults.boolean);
    ASSERT_EQ(42, defaults.integer);
    ASSERT_EQ(4.2, defaults.float_var);
    ASSERT_EQ(42, defaults.opt_integer);
    ASSERT_EQ(std::nullopt, defaults.opt_vec);
    ASSERT_EQ(false, defaults.boolean_default);
    ASSERT_EQ("", defaults.string_default);
    ASSERT_EQ(std::nullopt, defaults.opt_default);
    ASSERT_EQ("", defaults.no_default_string);
    ASSERT_TRUE(defaults.vec.empty());
    ASSERT_EQ(42, defaults.custom_integer);
    ASSERT_EQ(0, defaults.sub.int32);
    ASSERT_NE(nullptr, defaults.sub.object);

    // `std::vector<bool>` is a special case in C++ because it packs elements into single
    // bits rather than storing a real bool, so iterating it yields a temporary value
    // instead of a reference to an element.
    const std::vector<bool> bools = {true, false, true};
    auto lowered = proc_macro::uniffi::FfiConverterSequenceBool::lower(bools);
    auto roundtripped = proc_macro::uniffi::FfiConverterSequenceBool::lift(lowered);
    ASSERT_EQ(bools, roundtripped);

    ASSERT_EQ(42, proc_macro::double_with_default(21));
    ASSERT_EQ(30, proc_macro::sum_with_default(10, 20));

    auto object = proc_macro::ObjectWithDefaults::init(30);
    ASSERT_EQ(42, object->add_to_num(12));
    ASSERT_EQ(30, object->add_to_implicit_num(0));

    return 0;
}
