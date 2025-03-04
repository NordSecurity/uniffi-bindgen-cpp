use uniffi_bindgen::{interface::Literal, ComponentInterface};

use crate::bindings::cpp::{
    gen_cpp::filters::callback_interface_name, gen_cpp::filters::CppCodeOracle, CodeType,
};

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
    fn type_label(&self, _ci: &ComponentInterface) -> String {
        format!("std::shared_ptr<{}>", self.canonical_name())
    }

    fn canonical_name(&self) -> String {
        CppCodeOracle.class_name(&self.id)
    }

    fn literal(&self, _literal: &Literal, _ci: &ComponentInterface) -> String {
        unreachable!();
    }

    fn initialization_fn(&self) -> Option<String> {
        Some(format!(
            "uniffi::{}::init",
            callback_interface_name(&self.id).unwrap()
        ))
    }
}
