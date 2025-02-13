struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(RustBuffer buf);
    static RustBuffer lower(const {{ type_name }}& val);
    static {{ type_name }} read(RustStream &stream);
    static void write(RustStream &stream, const {{ type_name }}& value);
    static uint64_t allocation_size(const {{ type_name }} &val);
};
