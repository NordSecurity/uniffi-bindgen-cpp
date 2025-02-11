struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift({{ type_name }});
    static {{ type_name }} lower({{ type_name }});
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, {{ type_name }});
    static int32_t allocation_size({{ type_name }});
};
