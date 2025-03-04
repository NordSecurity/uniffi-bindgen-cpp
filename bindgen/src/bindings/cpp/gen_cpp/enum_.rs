use uniffi_bindgen::{backend::Literal, ComponentInterface};

use crate::bindings::cpp::{gen_cpp::filters::CppCodeOracle, CodeType};
#[derive(Debug)]
pub(crate) struct EnumCodeType {
    id: String,
}

impl EnumCodeType {
    pub(crate) fn new(id: String) -> Self {
        Self { id }
    }
}

impl CodeType for EnumCodeType {
    fn type_label(&self, ci: &ComponentInterface) -> String {
        if ci.is_name_used_as_error(&self.id) {
            format!("std::shared_ptr<{}>", self.canonical_name())
        } else {
            CppCodeOracle.class_name(&self.id)
        }
    }

    fn canonical_name(&self) -> String {
        CppCodeOracle.class_name(&self.id)
    }

    fn literal(&self, _: &Literal, _ci: &ComponentInterface) -> String {
        unreachable!();
    }
}
