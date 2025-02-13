struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(uint8_t);
    static uint8_t lower({{ type_name }});
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, {{ type_name }});
    static uint64_t allocation_size({{ type_name }});
};
