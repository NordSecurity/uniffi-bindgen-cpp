{%- let rec = ci.get_object_definition(name).unwrap() %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

struct {{ typ|ffi_converter_name }} {
    static {{ class_name }} lift(void *);
    static void *lower(const {{ class_name }} &);
    static {{ class_name }} read(uniffi::RustStream &);
    static void write({{ namespace }}::uniffi::RustStream &, const {{ type_name }} &);

    static int32_t allocation_size(const {{ class_name }} &);
};
