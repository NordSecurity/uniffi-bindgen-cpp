use std::sync::Arc;

#[derive(uniffi::Object)]
pub struct Object {
    pub value: i32,
}

#[uniffi::export]
impl Object {
    pub fn get_value(&self) -> i32 {
        self.value
    }
}

#[derive(uniffi::Record)]
pub struct Structure {
    #[uniffi(default = None)]
    pub optional_arc: Option<Arc<Object>>,
}

#[uniffi::export]
pub fn get_struct(value: i32) -> Structure {
    Structure {
        optional_arc: Some(Arc::new(Object { value })),
    }
}

#[uniffi::export]
pub fn struct_roundtrip(structure: Structure) -> Structure {
    structure
}

pub struct OptionalDefaults {
    pub maybe_int: Option<i32>,
    pub maybe_text: Option<String>,
    pub maybe_bool: Option<bool>,
    pub maybe_null: Option<i32>,
}

/// C++ types whose default value has to coincide with Rust's `Default`. The rest fail on generation instead.
#[derive(uniffi::Record)]
pub struct SupportedDefaults {
    #[uniffi(default)]
    pub int: i32,
    #[uniffi(default)]
    pub flag: bool,
    #[uniffi(default)]
    pub float: f64,
    #[uniffi(default)]
    pub text: String,
    #[uniffi(default)]
    pub bytes: Vec<u8>,
    #[uniffi(default)]
    pub maybe: Option<i32>,
    #[uniffi(default)]
    pub list: Vec<String>,
    #[uniffi(default)]
    pub table: std::collections::HashMap<String, i32>,
    #[uniffi(default)]
    pub maybe_object: Option<Arc<Object>>,
    #[uniffi(default)]
    pub enum_list: Vec<Shape>,
    #[uniffi(default)]
    pub enum_table: std::collections::HashMap<String, Shape>,
}

#[derive(uniffi::Enum)]
pub enum Shape {
    Circle { radius: f64 },
}

pub enum Colour {
    Red,
    Green,
}

pub struct OptionalEnumDefault {
    pub maybe_colour: Option<Colour>,
}

uniffi::include_scaffolding!("type_flattening");
