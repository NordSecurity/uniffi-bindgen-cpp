{%- let rec = ci.get_record_definition(name).unwrap() %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

struct {{ type_name }} {
    {%- for field in rec.fields() %}
    {{ field|type_name }} {{ field.name()|var_name }}
    {%- match field.default_value() %}
    {%- when Some with (literal) %} = {{ literal|literal_cpp(field) }};{%- else -%};
    {%- endmatch %}
    {%- endfor %}
};

namespace uniffi {
    struct {{ typ|ffi_converter_name }} {
        static ::{{ ci.namespace() }}::{{ class_name }} lift(RustBuffer);
        static RustBuffer lower(const ::{{ ci.namespace() }}::{{ class_name }} &);
        static ::{{ ci.namespace() }}::{{ class_name }} read(uniffi::RustStream &);
        static void write(uniffi::RustStream &, const {{ class_name }} &);

        static int32_t allocation_size(const {{ class_name }} &);
    };
}
