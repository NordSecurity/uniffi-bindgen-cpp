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

uniffi::include_scaffolding!("type_flattening");
