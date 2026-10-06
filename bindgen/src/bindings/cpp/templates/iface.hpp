{% call macros::docstring_value(interface_docstring, 0) %}
struct {{ interface_name }} {
    virtual ~{{ interface_name }}() {}

    {%- call macros::method_decls(methods, "virtual ", " = 0") %}

    {%- if ci.is_name_used_as_error(name) %}
    virtual void throw_underlying() = 0;
    {%- endif %}
};