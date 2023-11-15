{%- let obj = ci|get_object_definition(name) %}
{%- let class_name = type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}

namespace uniffi { struct {{ ffi_converter_name|class_name }}; }
struct {{ class_name }} {
    friend uniffi::{{ ffi_converter_name|class_name }};

    {{ class_name }}(const {{ class_name }} &) = default;
    {{ class_name }}({{ class_name }} &&) = default;

    {{ class_name }} &operator=(const {{ class_name }} &) = default;
    {{ class_name }} &operator=({{ class_name }} &&) = default;

    {% match obj.primary_constructor() %}
    {%- when Some with (ctor) -%}
    static std::unique_ptr<{{ class_name }}> init({% call macros::param_list(ctor) %});
    {%- else %}
    {%- endmatch -%}

    {% for ctor in obj.alternate_constructors() %}
    static std::unique_ptr<{{ class_name }}> {{ ctor.name() }}({% call macros::param_list(ctor) %});
    {%- endfor %}

    {% for method in obj.methods() %}
    {%- match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name }} {% else %}void {% endmatch %}
    {{- method.name()|fn_name }}({% call macros::param_list(method) %});
    {% endfor %}
private:
    {{ class_name }}(void *);
    {{ class_name }}() = delete;

    void *instance;
};
