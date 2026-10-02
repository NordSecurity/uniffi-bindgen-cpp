{%- let rec = ci.get_record_definition(name).unwrap() %}
{% call macros::docstring(rec, 0) %}
struct {{ type_name }} {
    {%- for field in rec.fields() %}
    {%- call macros::docstring(field, 4) %}
    {{ field|type_name(ci) }} {{ field.name()|var_name }}
    {%- match field.default_value() %}
    {%- when Some with (default) %} = {{ default|default_cpp(field, config.enum_style, ci) }};{%- else -%};
    {%- endmatch %}
    {%- endfor %}
    {%- call macros::method_decls(rec.methods(), "", " const") %}
    {%- call macros::uniffi_trait_decls(rec.uniffi_trait_methods(), type_name) %}
};
