#include "test_common.hpp"

#include <type_flattening.hpp>

int main()
{
    int value = 42;

    auto structure = type_flattening::get_struct(value);
    ASSERT_NE(structure.optional_arc, nullptr);
    ASSERT_EQ(structure.optional_arc->get_value(), value);

    auto roundtrip = type_flattening::struct_roundtrip(structure);
    ASSERT_NE(roundtrip.optional_arc, nullptr);
    ASSERT_EQ(roundtrip.optional_arc->get_value(), value);

    auto defaults = type_flattening::OptionalDefaults{};
    ASSERT_EQ(42, defaults.maybe_int);
    ASSERT_EQ("hello", defaults.maybe_text);
    ASSERT_EQ(true, defaults.maybe_bool);
    ASSERT_EQ(std::nullopt, defaults.maybe_null);

    auto enum_default = type_flattening::OptionalEnumDefault{};
    ASSERT_EQ(type_flattening::Colour::kRed, enum_default.maybe_colour);

    auto supported_defaults = type_flattening::SupportedDefaults{};
    ASSERT_EQ(0, supported_defaults.int_);
    ASSERT_EQ(false, supported_defaults.flag);
    ASSERT_EQ(0.0, supported_defaults.float_);
    ASSERT_EQ("", supported_defaults.text);
    ASSERT_EQ(std::nullopt, supported_defaults.maybe);
    ASSERT_EQ(nullptr, supported_defaults.maybe_object);
    ASSERT_TRUE(supported_defaults.bytes.empty());
    ASSERT_TRUE(supported_defaults.list.empty());
    ASSERT_TRUE(supported_defaults.table.empty());
    ASSERT_TRUE(supported_defaults.enum_list.empty());
    ASSERT_TRUE(supported_defaults.enum_table.empty());

    return 0;
}
