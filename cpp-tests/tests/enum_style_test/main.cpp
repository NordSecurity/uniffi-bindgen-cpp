#include <test_common.hpp>

#include <enum_style_test.hpp>

int main() {
    auto simple = enum_style_test::get_simple_enum();
    enum_style_test::set_simple_enum(simple);

    auto complex = enum_style_test::get_complex_enum();
    enum_style_test::set_complex_enum(complex);

    auto repeated = enum_style_test::roundtrip_repeated_dependencies({
        enum_style_test::SimpleEnum::VARIANT_ONE,
        enum_style_test::SimpleEnum::VARIANT_TWO,
    });
    ASSERT_EQ(repeated.first, enum_style_test::SimpleEnum::VARIANT_ONE);
    ASSERT_EQ(repeated.second, enum_style_test::SimpleEnum::VARIANT_TWO);

    enum_style_test::OptionalEnumDefault optional_enum_default;
    ASSERT_EQ(optional_enum_default.value, enum_style_test::SimpleEnum::VARIANT_ONE);
    auto roundtripped = enum_style_test::roundtrip_optional_enum_default(optional_enum_default);
    ASSERT_EQ(roundtripped.value, enum_style_test::SimpleEnum::VARIANT_ONE);

    return 0;
}
