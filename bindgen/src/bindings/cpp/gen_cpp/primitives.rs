use uniffi_bindgen::{
    backend::{CodeType, Literal},
    interface::Radix,
};
use paste::paste;

fn render_literal(literal: &Literal) -> String {
    match literal {
        Literal::Boolean(v) => {
            if *v {
                "true".into()
            } else {
                "false".into()
            }
        }
        Literal::String(s) => format!("\"{s}\""),
        Literal::Int(i, radix, _) => match radix {
            Radix::Octal => format!("{i:o}"),
            Radix::Decimal => format!("{i}"),
            Radix::Hexadecimal => format!("{i:#x}"),
        },
        Literal::UInt(i, radix, _) => match radix {
            Radix::Octal => format!("{i:o}U"),
            Radix::Decimal => format!("{i}U"),
            Radix::Hexadecimal => format!("{i:#x}U"),
        },
        Literal::Float(string, _type_) => string.clone(),

        _ => unreachable!("Literal"),
    }
}

macro_rules! impl_code_type_for_primitive {
    ($T:ty, $cpp_name:literal, $canonical_name:literal) => {
        paste! {
            #[derive(Debug)]
            pub(crate) struct $T;
            impl CodeType for $T  {
                fn type_label(&self) -> String {
                    $cpp_name.into()
                }

                fn canonical_name(&self) -> String {
                    $canonical_name.into()
                }

                fn literal(&self, literal: &Literal) -> String {
                    render_literal(&literal)
                }
            }
        }
    };
}

impl_code_type_for_primitive!(BooleanCodeType, "bool", "Bool");
impl_code_type_for_primitive!(StringCodeType, "std::string", "String");
impl_code_type_for_primitive!(BytesCodeType, "std::vector<uint8_t>", "Bytes");
impl_code_type_for_primitive!(Int8CodeType, "int8_t", "Int8");
impl_code_type_for_primitive!(Int16CodeType, "int16_t", "Int16");
impl_code_type_for_primitive!(Int32CodeType, "int32_t", "Int32");
impl_code_type_for_primitive!(Int64CodeType, "int64_t", "Int64");
impl_code_type_for_primitive!(UInt8CodeType, "uint8_t", "UInt8");
impl_code_type_for_primitive!(UInt16CodeType, "uint16_t", "UInt16");
impl_code_type_for_primitive!(UInt32CodeType, "uint32_t", "UInt32");
impl_code_type_for_primitive!(UInt64CodeType, "uint64_t", "UInt64");
impl_code_type_for_primitive!(Float32CodeType, "float", "Float");
impl_code_type_for_primitive!(Float64CodeType, "double", "Double");
