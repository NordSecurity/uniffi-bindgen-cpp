use uniffi_bindgen::{
    interface::{DefaultValue, Literal, Type},
    ComponentInterface,
};

use crate::bindings::cpp::{
    gen_cpp::filters::external_namespace_prefix, gen_cpp::filters::CppCodeOracle, CodeType,
};

#[derive(Debug)]
pub struct CustomCodeType {
    name: String,
    module_path: String,
    builtin: Type,
}

impl CustomCodeType {
    pub fn new(name: String, module_path: String, builtin: Type) -> Self {
        CustomCodeType {
            name,
            module_path,
            builtin,
        }
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

    // Defaults come from the wrapped builtin. Correct while the custom type is a typedef of it,
    // which stops being true once a `type_name` is configured.
    fn literal(&self, literal: &Literal, ci: &ComponentInterface) -> String {
        CppCodeOracle.find(&self.builtin).literal(literal, ci)
    }

    fn default(&self, default: &DefaultValue, ci: &ComponentInterface) -> String {
        CppCodeOracle.find(&self.builtin).default(default, ci)
    }
}
