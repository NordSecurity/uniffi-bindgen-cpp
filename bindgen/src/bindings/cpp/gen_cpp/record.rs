use uniffi_bindgen::backend::{CodeType, Literal};

use crate::bindings::cpp::gen_cpp::filters::CppCodeOracle;

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
    fn type_label(&self) -> String {
        CppCodeOracle.class_name(&self.id)
    }

    fn canonical_name(&self) -> String {
        format!("Type{}", self.id)
    }

    fn literal(&self, _literal: &Literal) -> String {
        unreachable!();
    }
}
