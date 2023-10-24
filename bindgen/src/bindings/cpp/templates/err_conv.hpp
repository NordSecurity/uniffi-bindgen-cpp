{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

struct {{ typ|ffi_converter_name }} {
    static {{ namespace }}::{{ class_name }} lift(RustBuffer buf);
    static RustBuffer lower(const {{ namespace }}::{{ class_name }} &);
    static {{ namespace }}::{{ class_name }} read({{ namespace }}::uniffi::RustStream &stream);
    static void write({{ namespace }}::uniffi::RustStream &stream, const {{ namespace }}::{{ class_name }} &);

    static int32_t allocation_size(const {{ namespace }}::{{ class_name }} &);
};
