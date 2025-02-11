use crate::bindings::cpp::CodeType;
use uniffi_bindgen::{backend::Type, interface::Literal, ComponentInterface};

use crate::bindings::cpp::gen_cpp::filters::CppCodeOracle;

#[derive(Debug)]
pub(crate) struct OptionalCodeType {
    inner: Type,
}

impl OptionalCodeType {
    pub(crate) fn new(inner: Type) -> Self {
        Self { inner }
    }

    pub(crate) fn can_dereference(inner_type: &Type, ci: &ComponentInterface) -> bool {
        match inner_type {
            Type::Object { .. } | Type::CallbackInterface { .. } => true,
            Type::Enum { name, .. } => ci.is_name_used_as_error(name),
            _ => false,
        }
    }
}

impl CodeType for OptionalCodeType {
    fn type_label(&self, ci: &ComponentInterface) -> String {
        if OptionalCodeType::can_dereference(&self.inner, ci) {
            CppCodeOracle.find(&self.inner).type_label(ci)
        } else {
            format!(
                "std::optional<{}>",
                CppCodeOracle.find(&self.inner).type_label(ci)
            )
        }
    }

    fn canonical_name(&self) -> String {
        format!(
            "Optional{}",
            CppCodeOracle.find(&self.inner).canonical_name(),
        )
    }

    fn literal(&self, literal: &Literal, ci: &ComponentInterface) -> String {
        match literal {
            Literal::None => "std::nullopt".into(),
            _ => CppCodeOracle.find(&self.inner).literal(literal, ci),
        }
    }
}

#[derive(Debug)]
pub(crate) struct SequenceCodeType {
    inner: Type,
}

impl SequenceCodeType {
    pub(crate) fn new(inner: Type) -> Self {
        Self { inner }
    }
}

impl CodeType for SequenceCodeType {
    fn type_label(&self, ci: &ComponentInterface) -> String {
        format!(
            "std::vector<{}>",
            CppCodeOracle.find(&self.inner).type_label(ci)
        )
    }

    fn canonical_name(&self) -> String {
        format!(
            "Sequence{}",
            CppCodeOracle.find(&self.inner).canonical_name(),
        )
    }

    fn literal(&self, literal: &Literal, ci: &ComponentInterface) -> String {
        match literal {
            Literal::EmptySequence => "{}".into(),
            _ => CppCodeOracle.find(&self.inner).literal(literal, ci),
        }
    }
}

#[derive(Debug)]
pub(crate) struct MapCodeType {
    key: Type,
    value: Type,
}

impl MapCodeType {
    pub(crate) fn new(key: Type, value: Type) -> Self {
        Self { key, value }
    }

    fn key(&self) -> &Type {
        &self.key
    }

    fn value(&self) -> &Type {
        &self.value
    }
}

impl CodeType for MapCodeType {
    fn type_label(&self, ci: &ComponentInterface) -> String {
        format!(
            "std::unordered_map<{}, {}>",
            CppCodeOracle.find(self.key()).type_label(ci),
            CppCodeOracle.find(self.value()).type_label(ci),
        )
    }

    fn canonical_name(&self) -> String {
        format!(
            "Map{}{}",
            CppCodeOracle.find(self.key()).canonical_name(),
            CppCodeOracle.find(self.value()).canonical_name(),
        )
    }

    fn literal(&self, literal: &Literal, ci: &ComponentInterface) -> String {
        match literal {
            Literal::EmptyMap => "{}".into(),
            _ => CppCodeOracle.find(&self.value).literal(literal, ci),
        }
    }
}
