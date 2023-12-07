{%- let iface = ci|get_callback_interface_definition(name) %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}
{% call macros::docstring(iface, 0) %}
struct {{ class_name }} {
    {%- for method in iface.methods() %}
    {% call macros::docstring(method, 4) %}
    virtual
    {%- match method.return_type() %}
    {% when Some with (return_type) %} {{ return_type|type_name }} {% else %} void {% endmatch -%}
    {{ method.name()|fn_name }}({% call macros::param_list(method) %}) = 0;
    {%- endfor %}

    virtual ~{{ class_name }}() = default;
};