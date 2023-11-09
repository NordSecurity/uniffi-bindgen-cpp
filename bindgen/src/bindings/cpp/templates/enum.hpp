{%- if e.is_flat() %}
enum class {{ type_name }}: int32_t {
    {% for variant in e.variants() -%}
    {{ variant|variant_name }} = {{ loop.index }}
    {%- if !loop.last %},
    {% endif -%}
    {% endfor %}
};
{% else %}
namespace uniffi {
    struct {{ ffi_converter_name }};
}

{%- let ffi_converter_name = typ|ffi_converter_name|class_name %}
struct {{ type_name }} {
    friend uniffi::{{ ffi_converter_name }};

    {% for variant in e.variants() -%}
    class {{ variant|variant_name }} {
        public:
        {%- for field in variant.fields() %}
        {{ field|type_name }} {{ field.name()|var_name }}
        {%- match field.default_value() %}
        {%- when Some with (literal) %} = {{ literal|literal_cpp(field) }};{%- else -%};
        {%- endmatch %}
        {%- endfor %}
    };
    {% endfor %}

    std::variant<{% for variant in e.variants() -%} {{ variant|variant_name }} {%- if !loop.last %}, {% endif -%} {% endfor %}> variant;
private:
    // {{ type_name }}();
    // {{ type_name }}(const {{ type_name }} &);
    // {{ type_name }}({{ type_name }} &&);
    // {{ type_name }} &operator=(const {{ type_name }} &);
    // {{ type_name }} &operator=({{ type_name }} &&);
};
{% endif %}