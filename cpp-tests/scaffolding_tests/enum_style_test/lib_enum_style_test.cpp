#include <lib_enum_style_test.hpp>

enum_style_test::SimpleEnum enum_style_test::get_simple_enum() {
    return enum_style_test::SimpleEnum::VARIANT_ONE;
}

void enum_style_test::set_simple_enum(enum_style_test::SimpleEnum) {
}

enum_style_test::ComplexEnum enum_style_test::get_complex_enum() {
    return enum_style_test::ComplexEnum { enum_style_test::ComplexEnum::VARIANT_ONE { 1 } };
}

void enum_style_test::set_complex_enum(enum_style_test::ComplexEnum) {
}


#include <enum_style_test_cpp_scaffolding.cpp>
