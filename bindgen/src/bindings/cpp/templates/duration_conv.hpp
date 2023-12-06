{%- let type_name = typ|type_name %}
struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(RustBuffer);
    static RustBuffer lower(const {{ type_name }} &);
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, const {{ type_name }} &);
    static int32_t allocation_size(const {{ type_name }} &);
};