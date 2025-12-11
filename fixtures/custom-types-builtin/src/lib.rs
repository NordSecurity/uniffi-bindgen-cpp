use std::collections::HashMap;

// Define the custom type structs
pub struct MyString(pub String);
pub struct CustomString(pub String);
pub struct Array(pub Vec<String>);
pub struct Bytes(pub Vec<u8>);
pub struct Table(pub HashMap<String, String>);
pub struct Boolean(pub bool);
pub struct Int8(pub i8);
pub struct Int16(pub i16);
pub struct Int32(pub i32);
pub struct Int64(pub i64);
pub struct UInt8(pub u8);
pub struct UInt16(pub u16);
pub struct UInt32(pub u32);
pub struct UInt64(pub u64);
pub struct Float(pub f32);
pub struct Double(pub f64);

// Define custom types using the new uniFFI 0.29.3 macro approach
uniffi::custom_newtype!(MyString, String);
uniffi::custom_newtype!(CustomString, String);
uniffi::custom_newtype!(Array, Vec<String>);
uniffi::custom_newtype!(Bytes, Vec<u8>);
uniffi::custom_newtype!(Table, HashMap<String, String>);
uniffi::custom_newtype!(Boolean, bool);
uniffi::custom_newtype!(Int8, i8);
uniffi::custom_newtype!(Int16, i16);
uniffi::custom_newtype!(Int32, i32);
uniffi::custom_newtype!(Int64, i64);
uniffi::custom_newtype!(UInt8, u8);
uniffi::custom_newtype!(UInt16, u16);
uniffi::custom_newtype!(UInt32, u32);
uniffi::custom_newtype!(UInt64, u64);
uniffi::custom_newtype!(Float, f32);
uniffi::custom_newtype!(Double, f64);

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
