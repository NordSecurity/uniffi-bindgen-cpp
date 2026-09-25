use uniffi_bindgen::{
    interface::{DefaultValue, Literal, Object, ObjectImpl},
    ComponentInterface,
};

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

    fn default(&self, default: &DefaultValue, ci: &ComponentInterface) -> String {
        match default {
            DefaultValue::Literal(literal) => self.literal(literal, ci),
            // An object is constructed through its primary constructor, which is emitted as a
            // static `init()`. Only a constructor taking no arguments can serve as the default.
            DefaultValue::Default => {
                let takes_no_args = ci
                    .get_object_definition(&self.id)
                    .and_then(Object::primary_constructor)
                    .is_some_and(|ctor| ctor.arguments().is_empty());

                if takes_no_args {
                    format!(
                        "{}{}::init()",
                        external_namespace_prefix(ci, &self.module_path),
                        self.canonical_name()
                    )
                } else {
                    unimplemented!(
                        "`#[uniffi(default)]` is not supported for {}: it requires a constructor \
                         taking no arguments",
                        self.type_label(ci)
                    )
                }
            }
        }
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
