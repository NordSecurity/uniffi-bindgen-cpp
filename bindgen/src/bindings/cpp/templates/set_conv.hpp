{%- let ffi_converter_name = typ|ffi_converter_name %}
struct {{ ffi_converter_name|class_name }} {
    static {{ type_name }} lift(RustBuffer);
    static RustBuffer lower(const {{ type_name }} &);
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, const {{ type_name }} &);
    static uint64_t allocation_size(const {{ type_name }} &);
};
