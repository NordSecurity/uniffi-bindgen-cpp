{% let type_name = typ|type_name(ci) %}
struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(RustBuffer);
    static RustBuffer lower(const {{ type_name }} &);
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, const {{ type_name }} &);
    static uint64_t allocation_size(const {{ type_name }} &);
};
