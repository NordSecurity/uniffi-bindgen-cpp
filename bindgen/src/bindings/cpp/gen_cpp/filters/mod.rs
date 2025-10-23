use askama;
use heck::{ToShoutySnakeCase, ToSnakeCase, ToUpperCamelCase};
// Backend filters module was removed in UniFFI 0.30
// pub(crate) use uniffi_bindgen::backend::filters::*;
use uniffi_bindgen::{
    interface::{Argument, AsType, CallbackInterface, Enum, Field, FfiType, Literal, Object, Type, Variant},
    ComponentInterface,
};
use uniffi_meta;

use crate::bindings::cpp::{
    gen_cpp::{
        callback_interface, compounds, custom, enum_, miscellany, object, primitives, record,
    },
    CodeType,
};

use super::EnumStyle;

type Result<T> = std::result::Result<T, askama::Error>;

const RESERVED_CPP_KEYWORDS: [&str; 98] = [
    "alignas",
    "alignof",
    "and",
    "and_eq",
    "asm",
    "atomic_cancel",
    "atomic_commit",
    "atomic_noexcept",
    "auto",
    "bitand",
    "bitor",
    "bool",
    "break",
    "case",
    "catch",
    "char",
    "char8_t",
    "char16_t",
    "char32_t",
    "class",
    "compl",
    "concept",
    "const",
    "consteval",
    "constexpr",
    "constinit",
    "const_cast",
    "continue",
    "contract_assert",
    "co_await",
    "co_return",
    "co_yield",
    "decltype",
    "default",
    "delete",
    "do",
    "double",
    "dynamic_cast",
    "else",
    "enum",
    "explicit",
    "export",
    "extern",
    "false",
    "float",
    "for",
    "friend",
    "goto",
    "if",
    "inline",
    "int",
    "long",
    "mutable",
    "namespace",
    "new",
    "noexcept",
    "not",
    "not_eq",
    "nullptr",
    "operator",
    "or",
    "or_eq",
    "private",
    "protected",
    "public",
    "reflexpr",
    "register",
    "reinterpret_cast",
    "requires",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "static_assert",
    "static_cast",
    "struct",
    "switch",
    "synchronized",
    "template",
    "this",
    "thread_local",
    "throw",
    "true",
    "try",
    "typedef",
    "typeid",
    "typename",
    "union",
    "unsigned",
    "using",
    "virtual",
    "void",
    "volatile",
    "wchar_t",
    "while",
    "xor",
    "xor_eq",
];

#[derive(Clone)]
pub(crate) struct CppCodeOracle;

impl CppCodeOracle {
    pub(crate) fn find(&self, typ: &Type) -> Box<dyn CodeType> {
        typ.clone().as_type().as_codetype()
    }

    pub(crate) fn class_name(&self, nm: &str) -> String {
        nm.to_string().to_upper_camel_case()
    }

    pub(crate) fn enum_variant_name(&self, nm: &str, style: &EnumStyle) -> String {
        match style {
            EnumStyle::Capitalized => nm.to_string().to_shouty_snake_case(),
            EnumStyle::Google => format!("k{}", nm.to_string().to_upper_camel_case()),
        }
    }

    pub(crate) fn fn_name(&self, nm: &str) -> String {
        nm.to_string().to_snake_case()
    }

    pub(crate) fn var_name(&self, nm: &str) -> String {
        let mut name = nm.to_string().to_snake_case();
        if RESERVED_CPP_KEYWORDS.contains(&&*name) {
            name.push('_');
        }
        name
    }

    pub(crate) fn object_names(&self, obj: &Object) -> (String, String) {
        let class_name = self.class_name(obj.name());
        if obj.has_callback_interface() {
            let impl_name = format!("{class_name}Impl");
            (class_name, impl_name)
        } else {
            (format!("I{class_name}"), class_name)
        }
    }
}

pub(crate) trait AsCodeType {
    fn as_codetype(&self) -> Box<dyn CodeType>;
}

// Implement AsCodeType for Type enum directly
impl AsCodeType for Type {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        match self {
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
            Type::Object { name, imp, .. } => Box::new(object::ObjectCodeType::new(name.clone(), *imp)),
            Type::Record { name, .. } => Box::new(record::RecordCodeType::new(name.clone())),
            Type::Enum { name, .. } => Box::new(enum_::EnumCodeType::new(name.clone())),
            Type::CallbackInterface { name, .. } => {
                Box::new(callback_interface::CallbackInterfaceCodeType::new(name.clone()))
            }
            Type::Optional { inner_type } => {
                Box::new(compounds::OptionalCodeType::new((**inner_type).clone()))
            }
            Type::Sequence { inner_type } => {
                Box::new(compounds::SequenceCodeType::new((**inner_type).clone()))
            }
            Type::Map {
                key_type,
                value_type,
            } => Box::new(compounds::MapCodeType::new((**key_type).clone(), (**value_type).clone())),
            // Type::External was removed in UniFFI 0.30
            // Type::External { .. } => todo!(),
            Type::Custom { name, .. } => Box::new(custom::CustomCodeType::new(name.clone())),
        }
    }
}

// Implement AsCodeType for &Type (Askama often passes references)
impl AsCodeType for &Type {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        (*self).as_codetype()
    }
}

// Implement AsCodeType for &&Type (double references)
impl AsCodeType for &&Type {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        (**self).as_codetype()
    }
}

// Implement AsCodeType for &&&Type (triple references)
impl AsCodeType for &&&Type {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        (***self).as_codetype()
    }
}

// Implement for Argument (which implements AsType)
impl AsCodeType for Argument {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        self.as_type().as_codetype()
    }
}

// Implement for &Argument
impl AsCodeType for &Argument {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        (*self).as_codetype()
    }
}

// Implement for &&Argument
impl AsCodeType for &&Argument {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        (**self).as_codetype()
    }
}

// Implement for Field
impl AsCodeType for Field {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        self.as_type().as_codetype()
    }
}

impl AsCodeType for &Field {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        (*self).as_codetype()
    }
}

impl AsCodeType for &&Field {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        (**self).as_codetype()
    }
}

// Implement for Enum
impl AsCodeType for &Enum {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        self.as_type().as_codetype()
    }
}

impl AsCodeType for &&Enum {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        (**self).as_codetype()
    }
}

// Implement for CallbackInterface
impl AsCodeType for &CallbackInterface {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        self.as_type().as_codetype()
    }
}

impl AsCodeType for &&CallbackInterface {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        (**self).as_codetype()
    }
}

// Implement for &Box<Type>
impl AsCodeType for &Box<Type> {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        self.as_ref().as_codetype()
    }
}

// Implement for &&Box<Type>
impl AsCodeType for &&Box<Type> {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        self.as_ref().as_codetype()
    }
}

// Implement for Box<Type> (owned) - Note: Type is uniffi_meta::Type
impl AsCodeType for Box<Type> {
    fn as_codetype(&self) -> Box<dyn CodeType> {
        self.as_ref().as_codetype()
    }
}

// Implement for common types that appear in templates and implement AsType
// (Add more as needed based on compilation errors)

pub(crate) fn to_lower_snake_case(s: &str) -> Result<String> {
    Ok(s.to_string().to_snake_case())
}

pub(crate) fn type_name(as_ct: impl AsCodeType, ci: &ComponentInterface) -> Result<String> {
    Ok(as_ct.as_codetype().type_label(ci))
}

pub(crate) fn ffi_converter_name(as_ct: impl AsCodeType) -> Result<String> {
    Ok(as_ct.as_codetype().ffi_converter_name())
}

// Helper for error converter (needs reference for match)
fn ffi_converter_name_for_ref(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(as_ct.as_codetype().ffi_converter_name())
}

pub(crate) fn ffi_error_converter_name(as_type: &(impl AsType + AsCodeType)) -> Result<String> {
    let mut name = ffi_converter_name_for_ref(as_type)?;

    if matches!(&as_type.as_type(), Type::Object { .. }) {
        name.push_str("__as_error");
    }

    Ok(name)
}

pub(crate) fn ffi_struct_name(nm: &str) -> Result<String> {
    Ok(format!("Uniffi{}", nm))
}

pub(crate) fn callback_interface_name(nm: &str) -> Result<String> {
    Ok(format!("UniffiCallbackInterface{}", nm))
}

pub(crate) fn canonical_name(as_ct: impl AsCodeType) -> Result<String> {
    Ok(as_ct.as_codetype().canonical_name())
}

pub(crate) fn fn_name(nm: &str) -> Result<String> {
    Ok(CppCodeOracle.fn_name(nm))
}

pub(crate) fn var_name(nm: &str) -> Result<String> {
    Ok(CppCodeOracle.var_name(nm))
}

pub(crate) fn object_names(obj: &Object) -> Result<(String, String)> {
    Ok(CppCodeOracle.object_names(obj))
}

// Helper to extract literal from DefaultValue
pub(crate) fn default_value_literal_cpp(
    default_value: &uniffi_bindgen::interface::DefaultValue,
    as_ct: &impl AsCodeType,
    enum_style: &EnumStyle,
    ci: &ComponentInterface,
) -> Result<String> {
    match default_value {
        uniffi_meta::DefaultValueMetadata::Literal(literal) => literal_cpp(literal, as_ct, enum_style, ci),
        uniffi_meta::DefaultValueMetadata::Default => {
            // Return the default value for the type
            Ok("{}".to_string()) // C++ default initialization
        }
    }
}

pub(crate) fn literal_cpp(
    literal: &Literal,
    as_ct: &impl AsCodeType,
    enum_style: &EnumStyle,
    ci: &ComponentInterface,
) -> Result<String> {
    match literal {
        Literal::Enum(name, _) => Ok(format!(
            "{}::{}",
            as_ct.as_codetype().type_label(ci),
            CppCodeOracle.enum_variant_name(&name, enum_style),
        )),
        _ => Ok(as_ct.as_codetype().literal(literal, ci)),
    }
}

pub(crate) fn lift_fn(as_ct: impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::lift",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn lower_fn(as_ct: impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::lower",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn read_fn(as_ct: impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::read",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn write_fn(as_ct: impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::write",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn allocation_size_fn(as_ct: impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::allocation_size",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn variant_name(v: &Variant, enum_style: &EnumStyle) -> Result<String> {
    Ok(CppCodeOracle.enum_variant_name(v.name(), enum_style))
}

// Core implementation
fn ffi_type_name_impl(ffi_type: &FfiType) -> Result<String> {
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
        // FfiType::RustArcPtr was removed in UniFFI 0.30, using Handle instead
        FfiType::Handle | FfiType::VoidPointer => "void *".into(),
        FfiType::MutReference(_) => "void *".into(), // Added in UniFFI 0.30
        FfiType::RustBuffer(_) => "RustBuffer".into(),
        FfiType::ForeignBytes => "ForeignBytes".into(),
        FfiType::Callback(_) => "void *".into(),
        FfiType::Struct(name) => ffi_struct_name(name)?,
        FfiType::RustCallStatus => "RustCallStatus*".into(),
        FfiType::Reference(typ) => format!("{} &", ffi_type_name_impl(typ)?),
    })
}

// Trait to allow accepting both owned FfiType and various reference levels
trait IntoFfiTypeRef {
    fn as_ffi_type_ref(&self) -> &FfiType;
}

impl IntoFfiTypeRef for FfiType {
    fn as_ffi_type_ref(&self) -> &FfiType {
        self
    }
}

impl IntoFfiTypeRef for &FfiType {
    fn as_ffi_type_ref(&self) -> &FfiType {
        self
    }
}

impl IntoFfiTypeRef for &&FfiType {
    fn as_ffi_type_ref(&self) -> &FfiType {
        self
    }
}

impl IntoFfiTypeRef for &&&FfiType {
    fn as_ffi_type_ref(&self) -> &FfiType {
        self
    }
}

impl IntoFfiTypeRef for Box<FfiType> {
    fn as_ffi_type_ref(&self) -> &FfiType {
        self.as_ref()
    }
}

impl IntoFfiTypeRef for &Box<FfiType> {
    fn as_ffi_type_ref(&self) -> &FfiType {
        self.as_ref()
    }
}

// Public filter function - accepts FfiType or any level of references
pub(crate) fn ffi_type_name(ffi_type: impl IntoFfiTypeRef) -> Result<String> {
    ffi_type_name_impl(ffi_type.as_ffi_type_ref())
}

pub(crate) fn class_name(nm: &str) -> Result<String> {
    Ok(CppCodeOracle.class_name(nm))
}

pub(crate) fn by_ref(ci: &ComponentInterface, arg: &Argument) -> bool {
    match arg.as_type() {
        Type::UInt8
        | Type::Int8
        | Type::UInt16
        | Type::Int16
        | Type::UInt32
        | Type::Int32
        | Type::UInt64
        | Type::Int64
        | Type::Float32
        | Type::Float64
        | Type::Boolean
        | Type::Optional { .. } => false,
        Type::Enum {
            module_path: _,
            name,
        } => match ci.get_enum_definition(&name) {
            Some(_enum) => _enum.is_flat(),
            None => false,
        },
        _ => true,
    }
}

pub(crate) fn parameter(arg: &Argument, ci: &ComponentInterface) -> Result<String> {
    Ok(match by_ref(ci, arg) {
        true => format!("const {} &{}", arg.as_codetype().type_label(ci), arg.name()),
        false => format!("{} {}", arg.as_codetype().type_label(ci), arg.name()),
    })
}

pub(crate) fn docstring(docstring: &str, spaces: &i32) -> Result<String> {
    let middle = textwrap::indent(&textwrap::dedent(docstring), " * ");
    let wrapped = format!("/**\n{middle}\n */");

    Ok(textwrap::indent(&wrapped, &" ".repeat(*spaces as usize)))
}

pub(crate) fn can_dereference_optional(type_: &Type, ci: &ComponentInterface) -> Result<bool> {
    let result = match type_ {
        Type::Optional { inner_type } => {
            compounds::OptionalCodeType::can_dereference(inner_type, ci)
        }
        _ => false,
    };
    Ok(result)
}

pub(crate) fn deref(type_: impl AsCodeType) -> Result<String> {
    // Simplified implementation - return empty string for most cases
    // In UniFFI 0.30, error handling is different so we don't need the * prefix
    Ok("".to_string())
}

// Filter functions for template access to ComponentInterface methods
// These were previously in backend::filters but that module was removed in UniFFI 0.30

pub(crate) fn get_record_definition<'a>(
    ci: &'a ComponentInterface,
    name: &str,
) -> Result<&'a uniffi_bindgen::interface::Record> {
    ci.get_record_definition(name)
        .ok_or_else(|| askama::Error::Custom(Box::new(std::io::Error::new(
            std::io::ErrorKind::NotFound,
            format!("Record '{}' not found", name)
        ))))
}

pub(crate) fn get_enum_definition<'a>(
    ci: &'a ComponentInterface,
    name: &str,
) -> Result<&'a uniffi_bindgen::interface::Enum> {
    ci.get_enum_definition(name)
        .ok_or_else(|| askama::Error::Custom(Box::new(std::io::Error::new(
            std::io::ErrorKind::NotFound,
            format!("Enum '{}' not found", name)
        ))))
}

pub(crate) fn get_callback_interface_definition<'a>(
    ci: &'a ComponentInterface,
    name: &str,
) -> Result<&'a uniffi_bindgen::interface::CallbackInterface> {
    ci.get_callback_interface_definition(name)
        .ok_or_else(|| askama::Error::Custom(Box::new(std::io::Error::new(
            std::io::ErrorKind::NotFound,
            format!("CallbackInterface '{}' not found", name)
        ))))
}

pub(crate) fn get_object_definition<'a>(
    ci: &'a ComponentInterface,
    name: &str,
) -> Result<&'a uniffi_bindgen::interface::Object> {
    ci.get_object_definition(name)
        .ok_or_else(|| askama::Error::Custom(Box::new(std::io::Error::new(
            std::io::ErrorKind::NotFound,
            format!("Object '{}' not found", name)
        ))))
}

// Helper filters to handle Type enum values directly from templates
// In UniFFI 0.30, Type is no longer wrapped, so templates pass Type values directly
pub(crate) fn type_lift_fn(type_: Type) -> Result<String> {
    Ok(format!("FfiConverter{}", CppCodeOracle.find(&type_).canonical_name()) + "::lift")
}

pub(crate) fn type_lower_fn(type_: Type) -> Result<String> {
    Ok(format!("FfiConverter{}", CppCodeOracle.find(&type_).canonical_name()) + "::lower")
}

pub(crate) fn type_read_fn(type_: Type) -> Result<String> {
    Ok(format!("FfiConverter{}", CppCodeOracle.find(&type_).canonical_name()) + "::read")
}

pub(crate) fn type_write_fn(type_: Type) -> Result<String> {
    Ok(format!("FfiConverter{}", CppCodeOracle.find(&type_).canonical_name()) + "::write")
}

pub(crate) fn type_allocation_size_fn(type_: Type) -> Result<String> {
    Ok(format!("FfiConverter{}", CppCodeOracle.find(&type_).canonical_name()) + "::allocation_size")
}

pub(crate) fn type_ffi_converter_name(type_: Type) -> Result<String> {
    Ok(format!("FfiConverter{}", CppCodeOracle.find(&type_).canonical_name()))
}
