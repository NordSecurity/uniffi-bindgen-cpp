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

    auto nested = enum_style_test::roundtrip_repeated_nested({
        {1}, {2}, {{3}, {4}}, {{"five", {5}}}, enum_style_test::Leaf{6},
    });
    ASSERT_EQ(nested.a.value, 1u);
    ASSERT_EQ(nested.b.value, 2u);
    ASSERT_EQ(nested.many.size(), 2u);
    ASSERT_EQ(nested.by_name.at("five").value, 5u);
    ASSERT_EQ(nested.maybe->value, 6u);

    auto variants = enum_style_test::roundtrip_repeated_variants(
        enum_style_test::RepeatedVariants::ONE{{7}, {8}});
    auto one = std::get<enum_style_test::RepeatedVariants::ONE>(variants.get_variant());
    ASSERT_EQ(one.x.value, 7u);
    ASSERT_EQ(one.y.value, 8u);

    return 0;
}
