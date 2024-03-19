struct {{ typ|ffi_converter_name }} {
    static {{ typ|type_name }} lift(RustBuffer);
    static RustBuffer lower(const {{ typ|type_name }} &);
    static {{ typ|type_name }} read(RustStream &);
    static void write(RustStream &, const {{ typ|type_name }} &);
    static int32_t allocation_size(const {{ typ|type_name }} &);
};
