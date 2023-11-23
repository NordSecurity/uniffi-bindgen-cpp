use uniffi_bindgen::backend::{CodeType, Literal};

use crate::bindings::cpp::gen_cpp::filters::CppCodeOracle;

#[derive(Debug)]
pub(crate) struct ObjectCodeType {
    id: String,
}

impl ObjectCodeType {
    pub(crate) fn new(id: String) -> Self {
        Self { id }
    }
}

impl CodeType for ObjectCodeType {
    fn type_label(&self) -> String {
        format!("std::shared_ptr<{}>", self.canonical_name())
    }

    fn canonical_name(&self) -> String {
        CppCodeOracle.class_name(&self.id)
    }

    fn literal(&self, _literal: &Literal) -> String {
        unreachable!();
    }
}
