{%- let type_name = e|type_name %}
{%- let class_name = type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let namespace = type_name|to_lower_snake_case %}
struct {{ ffi_converter_name }}{{ variant.name() }} {
    static {{ namespace }}::{{ variant.name() }} lift(RustBuffer buf, int32_t v);
    static RustBuffer lower(const {{ namespace }}::{{ variant.name() }} &);
    static {{ namespace }}::{{ variant.name() }} read(RustStream &stream, int32_t v);
    static void write(RustStream &stream, const {{ namespace }}::{{ variant.name() }} &);
    static int32_t allocation_size(const {{ namespace }}::{{ variant.name() }} &);
};
