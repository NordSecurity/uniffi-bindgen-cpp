{%- let iface = ci|get_callback_interface_definition(name) %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}

struct {{ class_name }} {
    {% for method in iface.methods() -%}
    virtual
    {%- match method.return_type() -%}
    {% when Some with (return_type) %} {{ return_type|type_name }} {% else %} {% endmatch -%}
    {{ method.name()|fn_name }}({% call macros::param_list_with_default(method) %}) = 0;
    {%- endfor %}
};
