enum SimpleEnum {
    VariantOne,
    VariantTwo,
    VariantThree,
}

enum ComplexEnum {
    VariantOne { num: u32 },
    VariantTwo { flt: f32 },
}

struct RepeatedDependencies {
    first: SimpleEnum,
    second: SimpleEnum,
}

struct OptionalEnumDefault {
    value: Option<SimpleEnum>,
}

fn get_simple_enum() -> SimpleEnum {
    SimpleEnum::VariantOne
}

fn set_simple_enum(_: SimpleEnum) {}

fn get_complex_enum() -> ComplexEnum {
    ComplexEnum::VariantOne { num: 42 }
}

fn set_complex_enum(_: ComplexEnum) {}

fn roundtrip_repeated_dependencies(value: RepeatedDependencies) -> RepeatedDependencies {
    value
}

fn roundtrip_optional_enum_default(value: OptionalEnumDefault) -> OptionalEnumDefault {
    value
}

uniffi::include_scaffolding!("enum_style_test");
