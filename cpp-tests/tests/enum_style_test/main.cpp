#include <test_common.hpp>

#include <enum_style_test.hpp>

int main() {
    auto simple = enum_style_test::get_simple_enum();
    enum_style_test::set_simple_enum(simple);

    auto complex = enum_style_test::get_complex_enum();
    enum_style_test::set_complex_enum(complex);

    return 0;
}
