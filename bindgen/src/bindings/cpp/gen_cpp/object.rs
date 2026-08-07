use uniffi_bindgen::{backend::Literal, interface::ObjectImpl, ComponentInterface};

use crate::bindings::cpp::{
    gen_cpp::filters::callback_interface_name, gen_cpp::filters::external_namespace_prefix,
    gen_cpp::filters::CppCodeOracle, CodeType,
};

#[derive(Debug)]
pub(crate) struct ObjectCodeType {
    id: String,
    imp: ObjectImpl,
    module_path: String,
}

impl ObjectCodeType {
    pub(crate) fn new(id: String, imp: ObjectImpl, module_path: String) -> Self {
        Self {
            id,
            imp,
            module_path,
        }
    }
}

impl CodeType for ObjectCodeType {
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
        self.imp.has_callback_interface().then(|| {
            format!(
                "uniffi::{}::init",
                callback_interface_name(&self.canonical_name()).unwrap()
            )
        })
    }
}
