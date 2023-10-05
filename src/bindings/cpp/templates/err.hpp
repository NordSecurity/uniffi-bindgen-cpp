{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

struct {{ class_name }}: std::runtime_error {
    {{ class_name }}(const std::string &what_arg) : std::runtime_error(what_arg) {}
};

{% for variant in e.variants() %}
struct {{ variant.name()|class_name }}: {{ type_name }} {
    {{ variant.name() }}(const std::string &what_arg) : {{ type_name }}(what_arg) {}
{% if !e.is_flat() %}
{%- endif %}
};
{% endfor %}
