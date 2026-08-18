use uniffi_bindgen::{backend::Literal, ComponentInterface};

use crate::bindings::cpp::{
    gen_cpp::filters::external_namespace_prefix, gen_cpp::filters::CppCodeOracle, CodeType,
};
#[derive(Debug)]
pub(crate) struct EnumCodeType {
    id: String,
    module_path: String,
}

impl EnumCodeType {
    pub(crate) fn new(id: String, module_path: String) -> Self {
        Self { id, module_path }
    }
}

impl CodeType for EnumCodeType {
    fn type_label(&self, ci: &ComponentInterface) -> String {
        if ci.is_name_used_as_error(&self.id) {
            format!(
                "std::shared_ptr<{}{}>",
                external_namespace_prefix(ci, &self.module_path),
                self.canonical_name()
            )
        } else {
            format!(
                "{}{}",
                external_namespace_prefix(ci, &self.module_path),
                CppCodeOracle.class_name(&self.id)
            )
        }
    }

    fn canonical_name(&self) -> String {
        CppCodeOracle.class_name(&self.id)
    }

    fn literal(&self, _: &Literal, _ci: &ComponentInterface) -> String {
        unreachable!();
    }
}
