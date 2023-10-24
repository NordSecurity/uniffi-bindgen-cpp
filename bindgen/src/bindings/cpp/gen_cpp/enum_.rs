use uniffi_bindgen::backend::{CodeType, Literal};

use crate::bindings::cpp::gen_cpp::filters::CppCodeOracle;

#[derive(Debug)]
pub(crate) struct EnumCodeType {
    id: String,
}

impl EnumCodeType {
    pub(crate) fn new(id: String) -> Self {
        Self { id }
    }
}

impl CodeType for EnumCodeType {
    fn type_label(&self) -> String {
        CppCodeOracle.class_name(&self.id)
    }

    fn canonical_name(&self) -> String {
        format!("Type{}", self.id)
    }

    fn literal(&self, literal: &Literal) -> String {
        if let Literal::Enum(v, _) = literal {
            format!(
                "{}::{}",
                self.type_label(),
                CppCodeOracle.enum_variant_name(v)
            )
        } else {
            unreachable!();
        }
    }
}

