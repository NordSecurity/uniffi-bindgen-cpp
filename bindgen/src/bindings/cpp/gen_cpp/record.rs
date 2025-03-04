use uniffi_bindgen::{backend::Literal, ComponentInterface};

use crate::bindings::cpp::{gen_cpp::filters::CppCodeOracle, CodeType};

#[derive(Debug)]
pub(crate) struct RecordCodeType {
    id: String,
}

impl RecordCodeType {
    pub(crate) fn new(id: String) -> Self {
        Self { id }
    }
}

impl CodeType for RecordCodeType {
    fn type_label(&self, _ci: &ComponentInterface) -> String {
        CppCodeOracle.class_name(&self.id)
    }

    fn canonical_name(&self) -> String {
        format!("Type{}", self.id)
    }

    fn literal(&self, _literal: &Literal, _ci: &ComponentInterface) -> String {
        unreachable!();
    }
}
