use paste::paste;
use std::collections::HashMap;

macro_rules! define_custom_builtin_type {
    ($custom:ty, $underlying:ty) => {
        paste! {
            pub struct $custom(pub $underlying);

            impl UniffiCustomTypeConverter for $custom {
                type Builtin = $underlying;

                fn into_custom(val: Self::Builtin) -> uniffi::Result<Self> {
                    Ok($custom(val))
                }

                fn from_custom(obj: Self) -> Self::Builtin {
                    obj.0
                }
            }
        }
    };
}

define_custom_builtin_type!(MyString, String);
define_custom_builtin_type!(CustomString, String);
define_custom_builtin_type!(Array, Vec<String>);
define_custom_builtin_type!(Bytes, Vec<u8>);
define_custom_builtin_type!(Table, HashMap<String, String>);
define_custom_builtin_type!(Boolean, bool);
define_custom_builtin_type!(Int8, i8);
define_custom_builtin_type!(Int16, i16);
define_custom_builtin_type!(Int32, i32);
define_custom_builtin_type!(Int64, i64);
define_custom_builtin_type!(UInt8, u8);
define_custom_builtin_type!(UInt16, u16);
define_custom_builtin_type!(UInt32, u32);
define_custom_builtin_type!(UInt64, u64);
define_custom_builtin_type!(Float, f32);
define_custom_builtin_type!(Double, f64);

pub struct CustomTypesBuiltin {
    string: MyString,
    custom_string: CustomString,
    array: Array,
    bytes: Bytes,
    table: Table,
    boolean: Boolean,
    int8: Int8,
    int16: Int16,
    int32: Int32,
    int64: Int64,
    uint8: UInt8,
    uint16: UInt16,
    uint32: UInt32,
    uint64: UInt64,
    flt: Float,
    dbl: Double,
}

pub fn get_custom_types_builtin() -> CustomTypesBuiltin {
    return CustomTypesBuiltin {
        string: MyString("Hello, world!".to_string()),
        custom_string: CustomString("Custom string".to_string()),
        array: Array(vec!["Hello, world!".to_string()]),
        bytes: Bytes(vec![0, 1, 2, 3, 4, 5, 6, 7, 8, 9]),
        table: Table(HashMap::from([("hello".to_string(), "world".to_string())])),
        boolean: Boolean(true),
        int8: Int8(i8::MAX),
        int16: Int16(i16::MAX),
        int32: Int32(i32::MAX),
        int64: Int64(i64::MAX),
        uint8: UInt8(u8::MAX),
        uint16: UInt16(u16::MAX),
        uint32: UInt32(u32::MAX),
        uint64: UInt64(u64::MAX),
        flt: Float(f32::MAX),
        dbl: Double(f64::MAX),
    };
}

pub fn return_custom_types_builtin(custom_types: CustomTypesBuiltin) -> CustomTypesBuiltin {
    custom_types
}

uniffi::include_scaffolding!("custom_types_builtin");
