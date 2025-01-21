use uniffi_bindgen::{
    backend::{CodeType, Literal},
    interface::ObjectImpl,
};

use crate::bindings::cpp::gen_cpp::filters::CppCodeOracle;

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
        self.imp
            .has_callback_interface()
            .then(|| format!("uniffi::UniffiCallbackInterface{}::init", self.canonical_name()))
    }
}
