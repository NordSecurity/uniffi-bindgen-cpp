use uniffi_bindgen::ComponentInterface;

use crate::bindings::cpp::{gen_cpp::filters::external_namespace_prefix, CodeType};

#[derive(Debug)]
pub struct CustomCodeType {
    name: String,
    module_path: String,
}

impl CustomCodeType {
    pub fn new(name: String, module_path: String) -> Self {
        CustomCodeType { name, module_path }
    }
}

impl CodeType for CustomCodeType {
    fn type_label(&self, ci: &ComponentInterface) -> String {
        format!(
            "{}{}",
            external_namespace_prefix(ci, &self.module_path),
            self.name
        )
    }

    fn canonical_name(&self) -> String {
        format!("Type{}", self.name)
    }
}
