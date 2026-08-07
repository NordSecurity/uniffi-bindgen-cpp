use uniffi_bindgen::{interface::Literal, ComponentInterface};

use crate::bindings::cpp::{
    gen_cpp::filters::{callback_interface_name, external_namespace_prefix, CppCodeOracle},
    CodeType,
};

#[derive(Debug)]
pub(crate) struct CallbackInterfaceCodeType {
    id: String,
    module_path: String,
}

impl CallbackInterfaceCodeType {
    pub(crate) fn new(id: String, module_path: String) -> Self {
        Self { id, module_path }
    }
}

impl CodeType for CallbackInterfaceCodeType {
    fn type_label(&self, ci: &ComponentInterface) -> String {
        format!(
            "std::shared_ptr<{}{}>",
            external_namespace_prefix(ci, &self.module_path),
            self.canonical_name()
        )
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
