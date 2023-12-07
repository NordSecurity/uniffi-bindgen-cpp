{%- let type_name = typ|type_name %}
struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(uint8_t);
    static uint8_t lower({{ type_name }});
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, {{ type_name }});
    static int32_t allocation_size({{ type_name }});
};