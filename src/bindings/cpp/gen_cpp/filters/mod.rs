use askama;
use heck::{ToShoutySnakeCase, ToSnakeCase, ToUpperCamelCase};
use uniffi_bindgen::{
    backend::{CodeType}, interface::{AsType, FfiType, Literal, Type, Variant},
};
pub(crate) use uniffi_bindgen::backend::filters::*;

use crate::bindings::cpp::gen_cpp::{
    compounds, enum_, miscellany, object, primitives, record,
};

type Result<T> = std::result::Result<T, askama::Error>;

#[derive(Clone)]
pub(crate) struct CppCodeOracle;

impl CppCodeOracle {
    pub(crate) fn find(&self, typ: &Type) -> Box<dyn CodeType> {
        typ.clone().as_type().as_codetype()
    }

    pub(crate) fn class_name(&self, nm: &str) -> String {
        nm.to_string().to_upper_camel_case()
    }

    pub(crate) fn enum_variant_name(&self, nm: &str) -> String {
        nm.to_string().to_shouty_snake_case()
    }

    pub(crate) fn fn_name(&self, nm: &str) -> String {
        nm.to_string().to_snake_case()
    }

    pub(crate) fn var_name(&self, nm: &str) -> String {
        nm.to_string().to_snake_case()
    }
}

pub(crate) trait AsCodeType {
    fn as_codetype(&self) -> Box<dyn CodeType>;
}

impl<T: AsType> AsCodeType for T {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        match self.as_type() {
            Type::UInt8 => Box::new(primitives::UInt8CodeType),
            Type::Int8 => Box::new(primitives::Int8CodeType),
            Type::UInt16 => Box::new(primitives::UInt16CodeType),
            Type::Int16 => Box::new(primitives::Int16CodeType),
            Type::UInt32 => Box::new(primitives::UInt32CodeType),
            Type::Int32 => Box::new(primitives::Int32CodeType),
            Type::UInt64 => Box::new(primitives::UInt64CodeType),
            Type::Int64 => Box::new(primitives::Int64CodeType),
            Type::Float32 => Box::new(primitives::Float32CodeType),
            Type::Float64 => Box::new(primitives::Float64CodeType),
            Type::Boolean => Box::new(primitives::BooleanCodeType),
            Type::String => Box::new(primitives::StringCodeType),
            Type::Bytes => Box::new(primitives::BytesCodeType),
            Type::Timestamp => Box::new(miscellany::TimestampCodeType),
            Type::Duration => Box::new(miscellany::DurationCodeType),
            Type::Object { name, .. } => Box::new(object::ObjectCodeType::new(name)),
            Type::ForeignExecutor => todo!(),
            Type::Record(id) => Box::new(record::RecordCodeType::new(id)),
            Type::Enum(id) => Box::new(enum_::EnumCodeType::new(id)),
            Type::CallbackInterface(_) => todo!(),
            Type::Optional(inner) => Box::new(compounds::OptionalCodeType::new(*inner)),
            Type::Sequence(inner) => Box::new(compounds::SequenceCodeType::new(*inner)),
            Type::Map(key, value) => Box::new(compounds::MapCodeType::new(*key, *value)),
            Type::External { .. } => todo!(),
            Type::Custom { .. } => todo!(),
        }
    }
}

pub(crate) fn type_name(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(as_ct.as_codetype().type_label())
}

pub(crate) fn ffi_converter_name(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(as_ct.as_codetype().ffi_converter_name())
}

pub(crate) fn canonical_name(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(as_ct.as_codetype().canonical_name())
}

pub(crate) fn fn_name(nm: &str) -> Result<String> {
    Ok(CppCodeOracle.fn_name(nm))
}

pub(crate) fn var_name(nm: &str) -> Result<String> {
    Ok(CppCodeOracle.var_name(nm))
}

pub(crate) fn literal_cpp(literal: &Literal, as_ct: &impl AsCodeType) -> Result<String> {
    Ok(as_ct.as_codetype().literal(literal))
}

pub(crate) fn lift_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!("{}::lift", as_ct.as_codetype().ffi_converter_name()))
}

pub(crate) fn lower_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!("{}::lower", as_ct.as_codetype().ffi_converter_name()))
}

pub(crate) fn read_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!("{}::read", as_ct.as_codetype().ffi_converter_name()))
}

pub(crate) fn write_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!("{}::write", as_ct.as_codetype().ffi_converter_name()))
}

pub(crate) fn allocation_size_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!("{}::allocation_size", as_ct.as_codetype().ffi_converter_name()))
}

pub(crate) fn variant_name(v: &Variant) -> Result<String> {
    Ok(CppCodeOracle.enum_variant_name(v.name()))
}

pub(crate) fn ffi_type_name(ffi_type: &FfiType) -> Result<String> {
    Ok(match ffi_type {
        FfiType::UInt8 => "uint8_t".into(),
        FfiType::Int8 => "int8_t".into(),
        FfiType::UInt16 => "uint16_t".into(),
        FfiType::Int16 => "int16_t".into(),
        FfiType::UInt32 => "uint32_t".into(),
        FfiType::Int32 => "int32_t".into(),
        FfiType::UInt64 => "uint64_t".into(),
        FfiType::Int64 => "int64_t".into(),
        FfiType::Float32 => "float".into(),
        FfiType::Float64 => "double".into(),
        FfiType::RustArcPtr(_) => "void *".into(),
        FfiType::RustBuffer(_) => "RustBuffer".into(),
        FfiType::ForeignBytes => "ForeignBytes".into(),
        FfiType::ForeignCallback => "ForeignCallback".into(),
        FfiType::ForeignExecutorCallback => "UniFfiForeignExecutorCallback".into(),
        FfiType::ForeignExecutorHandle => "std::size_t".into(),
        FfiType::FutureCallback { return_type } => todo!(),
        FfiType::FutureCallbackData => todo!(),
    })
}

pub(crate) fn class_name(nm: &str) -> Result<String> {
    Ok(CppCodeOracle.class_name(nm))
}
