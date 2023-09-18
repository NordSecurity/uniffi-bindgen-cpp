{% let rec = ci.get_record_definition(name).unwrap() %}

struct {{ type_name }} {
    {%- for field in rec.fields() %}
    {{ field|type_name }} {{ field.name()|var_name }};
    {%- match field.default_value() %}
    {%- when Some with (literal) %} = {{ literal|literal_cpp(field) }}
    {%- else %}
    {%- endmatch %}
    {%- endfor %}
}
