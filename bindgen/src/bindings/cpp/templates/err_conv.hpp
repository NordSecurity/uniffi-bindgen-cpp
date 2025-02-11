{%- let class_name = typ|canonical_name %}
struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(RustBuffer buf);
    static RustBuffer lower(const {{ class_name }} &);
    static {{ type_name }} read(RustStream &stream);
    static void write(RustStream &stream, const {{ class_name }} &);
    static int32_t allocation_size(const {{ class_name }} &);
};
