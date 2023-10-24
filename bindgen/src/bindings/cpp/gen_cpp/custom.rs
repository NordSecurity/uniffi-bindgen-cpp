use uniffi_bindgen::backend::CodeType;

#[derive(Debug)]
pub struct CustomCodeType {
    name: String,
}

impl CustomCodeType {
    pub fn new(name: String) -> Self {
        CustomCodeType { name }
    }
}

impl CodeType for CustomCodeType {
    fn type_label(&self) -> String {
        self.name.clone()
    }

    fn canonical_name(&self) -> String {
        format!("Type{}", self.name)
    }
}
