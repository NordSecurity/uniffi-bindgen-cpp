{%- let rec = ci.get_object_definition(name).unwrap() %}
{%- let type_name = typ|type_name %}

struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(void *);
    static void *lower(const {{ type_name }} &);
    static {{ type_name }} read(uniffi::RustStream &);
    static void write({{ namespace }}::uniffi::RustStream &, const {{ type_name }} &);

    static int32_t allocation_size(const {{ type_name }} &);
};
