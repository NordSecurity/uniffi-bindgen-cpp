{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}
struct {{ typ|ffi_converter_name }} {
    static {{ class_name }} lift(RustBuffer);
    static RustBuffer lower(const {{ class_name }} &);
    static {{ class_name }} read(uniffi::RustStream &);
    static void write(RustStream &, const {{ class_name }} &);
    static int32_t allocation_size(const {{ class_name }} &);
};