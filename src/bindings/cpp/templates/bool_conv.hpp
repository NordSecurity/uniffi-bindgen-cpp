{%- let type_name = typ|type_name %}

struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift();
    static uint8_t lower({{ type_name }});
    static {{ type_name }} read(::{{ namespace }}::uniffi::RustStream &);
    static void write(::{{ namespace }}::uniffi::RustStream &, {{ type_name }});
    static int32_t allocation_size({{ type_name }});
};
