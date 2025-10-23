{%- let rec = ci|get_record_definition(name) %}
{% call macros::docstring(rec, 0) %}
struct {{ type_name }} {
    {%- for field in rec.fields() %}
    {%- call macros::docstring(field, 4) %}
    {{ field|type_name(ci) }} {{ field.name()|var_name }}
    {%- match field.default_value() %}
    {%- when Some with (default_val) %} = {{ default_val|default_value_literal_cpp(field, config.enum_style, ci) }};{%- else -%};
    {%- endmatch %}
    {%- endfor %}
};
