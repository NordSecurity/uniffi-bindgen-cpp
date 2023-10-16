use uniffi_bindgen::backend::CodeType;

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
        CppCodeOracle.class_name(&self.id)
    }

    fn canonical_name(&self) -> String {
        format!("CallbackInterface{}", self.type_label())
    }
}
