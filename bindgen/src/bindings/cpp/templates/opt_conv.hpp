{%- let type_name = typ|type_name %}

struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(RustBuffer buf);
    static RustBuffer lower(const {{ type_name }}& val);
    static {{ type_name }} read({{ namespace }}::uniffi::RustStream &stream);
    static void write({{ namespace }}::uniffi::RustStream &stream, const {{ type_name }}& value);
    static int32_t allocation_size(const {{ type_name }} &val);
};
