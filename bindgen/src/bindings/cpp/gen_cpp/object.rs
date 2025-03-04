use uniffi_bindgen::{backend::Literal, interface::ObjectImpl, ComponentInterface};

use crate::bindings::cpp::{
    gen_cpp::filters::callback_interface_name, gen_cpp::filters::CppCodeOracle, CodeType,
};

#[derive(Debug)]
pub(crate) struct ObjectCodeType {
    id: String,
    imp: ObjectImpl,
}

impl ObjectCodeType {
    pub(crate) fn new(id: String, imp: ObjectImpl) -> Self {
        Self { id, imp }
    }
}

impl CodeType for ObjectCodeType {
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
        self.imp.has_callback_interface().then(|| {
            format!(
                "uniffi::{}::init",
                callback_interface_name(&self.canonical_name()).unwrap()
            )
        })
    }
}
