{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let type_name = typ|type_name %}

struct {{ ffi_converter_name|class_name }} {
    static {{ type_name }} lift(RustBuffer);
    static RustBuffer lower(const {{ type_name }} &);
    static {{ type_name }} read({{ namespace }}::uniffi::RustStream &);
    static void write({{ namespace }}::uniffi::RustStream &, const {{ type_name }} &);

    static int32_t allocation_size(const {{ type_name }} &);
};
