#[derive(uniffi::Record, Clone, Debug)]
pub struct RustRecord {
    pub rust_field: i32,
}

#[derive(uniffi::Enum, Clone, Debug)]
pub enum RustEnum {
    RustVariant { rust_variant_field: i32 },
    OtherVariant,
}

#[derive(uniffi::Object)]
pub struct RustObject {
    value: i32,
}

#[uniffi::export]
impl RustObject {
    #[uniffi::constructor]
    pub fn new(value: i32) -> Self {
        Self { value }
    }

    pub fn rust_method(&self, rust_arg: i32) -> i32 {
        self.value + rust_arg
    }
}

#[uniffi::export]
pub fn rust_function(rust_arg: i32) -> RustRecord {
    RustRecord {
        rust_field: rust_arg,
    }
}

#[uniffi::export]
pub fn make_rust_enum() -> RustEnum {
    RustEnum::RustVariant {
        rust_variant_field: 7,
    }
}

#[uniffi::export]
pub fn needs_escaping(value: i32) -> i32 {
    value
}

uniffi::setup_scaffolding!("renaming");
