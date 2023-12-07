{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}
struct {{ typ|ffi_converter_name }} {
    static std::unique_ptr<{{ class_name }}> lift(RustBuffer buf);
    static RustBuffer lower(const {{ class_name }} &);
    static std::unique_ptr<{{ class_name }}> read(RustStream &stream);
    static void write(RustStream &stream, const {{ class_name }} &);
    static int32_t allocation_size(const {{ class_name }} &);
};