use uniffi_bindgen::{
    interface::{AsType, DefaultValue, Literal, Type},
    ComponentInterface,
};

use crate::bindings::cpp::{
    gen_cpp::filters::external_namespace_prefix, gen_cpp::filters::CppCodeOracle, CodeType,
};

/// Whether `T()` is a valid way to construct this record. It zeroes every field lacking an
/// initialiser of its own.
fn is_value_initialisable(name: &str, ci: &ComponentInterface) -> bool {
    ci.get_record_definition(name).is_some_and(|record| {
        record.fields().iter().all(|field| {
            field.default_value().is_some()
                || match field.as_type() {
                    // 0 is no variant since they are numbered from 1. A non-flat enum also keeps
                    // its default constructor private.
                    Type::Enum { .. } => false,
                    // 0 leaves a null handle, which `lower` dereferences.
                    Type::Object { .. } | Type::CallbackInterface { .. } => false,
                    Type::Record { name, .. } => is_value_initialisable(&name, ci),
                    Type::Custom { .. } => false,
                    _ => true,
                }
        })
    })
}

#[derive(Debug)]
pub(crate) struct RecordCodeType {
    id: String,
    module_path: String,
}

impl RecordCodeType {
    pub(crate) fn new(id: String, module_path: String) -> Self {
        Self { id, module_path }
    }
}

impl CodeType for RecordCodeType {
    fn type_label(&self, ci: &ComponentInterface) -> String {
        format!(
            "{}{}",
            external_namespace_prefix(ci, &self.module_path),
            CppCodeOracle.class_name(&self.id)
        )
    }

    fn canonical_name(&self) -> String {
        format!("Type{}", self.id)
    }

    fn literal(&self, _literal: &Literal, _ci: &ComponentInterface) -> String {
        unreachable!();
    }

    fn default(&self, default: &DefaultValue, ci: &ComponentInterface) -> String {
        match default {
            DefaultValue::Literal(literal) => self.literal(literal, ci),
            DefaultValue::Default if is_value_initialisable(&self.id, ci) => {
                format!("{}()", self.type_label(ci))
            }
            DefaultValue::Default => unimplemented!(
                "`#[uniffi(default)]` is not supported for {}: it has a field with no default \
                 of its own that cannot be value-initialised",
                self.type_label(ci)
            ),
        }
    }
}
