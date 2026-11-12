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

struct RepeatedDependencies {
    first: SimpleEnum,
    second: SimpleEnum,
}

struct Leaf {
    value: u32,
}

struct RepeatedNested {
    a: Leaf,
    b: Leaf,
    many: Vec<Leaf>,
    by_name: std::collections::HashMap<String, Leaf>,
    maybe: Option<Leaf>,
}

enum RepeatedVariants {
    One { x: Leaf, y: Leaf },
    Two { z: Leaf, zs: Vec<Leaf> },
}

fn roundtrip_repeated_dependencies(value: RepeatedDependencies) -> RepeatedDependencies {
    value
}

fn roundtrip_repeated_nested(value: RepeatedNested) -> RepeatedNested {
    value
}

fn roundtrip_repeated_variants(value: RepeatedVariants) -> RepeatedVariants {
    value
}

uniffi::include_scaffolding!("enum_style_test");
