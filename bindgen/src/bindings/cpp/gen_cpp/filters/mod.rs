use askama;
use heck::{ToShoutySnakeCase, ToSnakeCase, ToUpperCamelCase};
pub(crate) use uniffi_bindgen::backend::filters::*;
use uniffi_bindgen::{
    interface::{Argument, AsType, FfiType, Literal, Object, Type, Variant},
    ComponentInterface,
};

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
            Type::Object { name, imp, .. } => Box::new(object::ObjectCodeType::new(name, imp)),
            Type::Record { name, .. } => Box::new(record::RecordCodeType::new(name)),
            Type::Enum { name, .. } => Box::new(enum_::EnumCodeType::new(name)),
            Type::CallbackInterface { name, .. } => {
                Box::new(callback_interface::CallbackInterfaceCodeType::new(name))
            }
            Type::Optional { inner_type } => {
                Box::new(compounds::OptionalCodeType::new(*inner_type))
            }
            Type::Sequence { inner_type } => {
                Box::new(compounds::SequenceCodeType::new(*inner_type))
            }
            Type::Map {
                key_type,
                value_type,
            } => Box::new(compounds::MapCodeType::new(*key_type, *value_type)),
            Type::External { .. } => todo!(),
            Type::Custom { name, .. } => Box::new(custom::CustomCodeType::new(name)),
        }
    }
}

pub(crate) fn to_lower_snake_case(s: &str) -> Result<String> {
    Ok(s.to_string().to_snake_case())
}

pub(crate) fn type_name(as_ct: &impl AsCodeType, ci: &ComponentInterface) -> Result<String> {
    Ok(as_ct.as_codetype().type_label(ci))
}

pub(crate) fn ffi_converter_name(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(as_ct.as_codetype().ffi_converter_name())
}

pub(crate) fn ffi_error_converter_name(as_type: &impl AsType) -> Result<String> {
    let mut name = ffi_converter_name(as_type)?;

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

pub(crate) fn canonical_name(as_ct: &impl AsCodeType) -> Result<String> {
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

pub(crate) fn lift_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::lift",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn lower_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::lower",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn read_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::read",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn write_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::write",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn allocation_size_fn(as_ct: &impl AsCodeType) -> Result<String> {
    Ok(format!(
        "{}::allocation_size",
        as_ct.as_codetype().ffi_converter_name()
    ))
}

pub(crate) fn variant_name(v: &Variant, enum_style: &EnumStyle) -> Result<String> {
    Ok(CppCodeOracle.enum_variant_name(v.name(), enum_style))
}

pub(crate) fn ffi_type_name(ffi_type: &FfiType) -> Result<String> {
    Ok(match ffi_type {
        FfiType::UInt8 => "uint8_t".into(),
        FfiType::Int8 => "int8_t".into(),
        FfiType::UInt16 => "uint16_t".into(),
        FfiType::Int16 => "int16_t".into(),
        FfiType::UInt32 => "uint32_t".into(),
        FfiType::Int32 => "int32_t".into(),
        FfiType::UInt64 | FfiType::Handle => "uint64_t".into(),
        FfiType::Int64 => "int64_t".into(),
        FfiType::Float32 => "float".into(),
        FfiType::Float64 => "double".into(),
        FfiType::RustArcPtr(_) | FfiType::VoidPointer => "void *".into(),
        FfiType::RustBuffer(_) => "RustBuffer".into(),
        FfiType::ForeignBytes => "ForeignBytes".into(),
        FfiType::Callback(_) => "void *".into(),
        FfiType::Struct(name) => ffi_struct_name(name)?,
        FfiType::RustCallStatus => "RustCallStatus*".into(),
        FfiType::Reference(typ) => format!("{} &", ffi_type_name(typ)?),
    })
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

pub(crate) fn deref(type_: Type, ci: &ComponentInterface) -> Result<String> {
    if let Type::Enum { name, .. } = type_ {
        if ci.is_name_used_as_error(&name) {
            return Ok("*".to_string());
        }
    }
    Ok("".to_string())
}
