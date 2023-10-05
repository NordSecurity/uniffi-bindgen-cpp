{%- let rec = ci.get_record_definition(name).unwrap() %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

struct {{ typ|ffi_converter_name }} {
    static {{ namespace }}::{{ class_name }} lift(RustBuffer);
    static RustBuffer lower(const {{ namespace }}::{{ class_name }} &);
    static {{ namespace }}::{{ class_name }} read(uniffi::RustStream &);
    static void write({{ namespace }}::uniffi::RustStream &, const {{ class_name }} &);

    static int32_t allocation_size(const {{ class_name }} &);
};
