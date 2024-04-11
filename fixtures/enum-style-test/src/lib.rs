enum SimpleEnum {
    VariantOne,
    VariantTwo,
    VariantThree,
}

enum ComplexEnum {
    VariantOne { num: u32 },
    VariantTwo { flt: f32 },
}

fn get_simple_enum() -> SimpleEnum {
    SimpleEnum::VariantOne
}

fn set_simple_enum(_: SimpleEnum) {}

fn get_complex_enum() -> ComplexEnum {
    ComplexEnum::VariantOne { num: 42 }
}

fn set_complex_enum(_: ComplexEnum) {}

uniffi::include_scaffolding!("enum_style_test");
