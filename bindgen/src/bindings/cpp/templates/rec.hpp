{%- let rec = ci|get_record_definition(name) %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

{%- call macros::docstring(rec, 0) %}
struct {{ type_name }} {
    {%- for field in rec.fields() %}
    {%- call macros::docstring(field, 4) %}
    {{ field|type_name }} {{ field.name()|var_name }}
    {%- match field.default_value() %}
    {%- when Some with (literal) %} = {{ literal|literal_cpp(field) }};{%- else -%};
    {%- endmatch %}
    {%- endfor %}
};
