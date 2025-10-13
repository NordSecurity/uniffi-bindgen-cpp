{%- let rec = ci.get_record_definition(name).unwrap() %}
{% call macros::docstring(rec, 0) %}
struct {{ type_name }} {
    {%- for field in rec.fields() %}
    {%- call macros::docstring(field, 4) %}
    {{ field|type_name(ci) }} {{ field.name()|var_name }}
    {%- match field.default_value() %}
    {%- when Some with (literal) %} = {{ literal|literal_cpp(field, config.enum_style, ci) }};{%- else -%};
    {%- endmatch %}
    {%- endfor %}
};
