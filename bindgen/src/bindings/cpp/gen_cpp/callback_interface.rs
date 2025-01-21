use uniffi_bindgen::{backend::CodeType, interface::Literal};

use crate::bindings::cpp::gen_cpp::filters::CppCodeOracle;

#[derive(Debug)]
pub(crate) struct CallbackInterfaceCodeType {
    id: String,
}

impl CallbackInterfaceCodeType {
    pub(crate) fn new(id: String) -> Self {
        Self { id }
    }
}

impl CodeType for CallbackInterfaceCodeType {
    fn type_label(&self) -> String {
        format!("std::shared_ptr<{}>", self.canonical_name())
    }

    fn canonical_name(&self) -> String {
        CppCodeOracle.class_name(&self.id)
    }

    fn literal(&self, _literal: &Literal) -> String {
        unreachable!();
    }

    fn initialization_fn(&self) -> Option<String> {
        Some(format!("uniffi::UniffiCallbackInterface{}::init", self.id))
    }
}
