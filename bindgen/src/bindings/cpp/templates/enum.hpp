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

    {% for variant in e.variants() %}
    struct {{ variant|variant_name }} {
        {%- for field in variant.fields() %}
        {{ field|type_name }} {{ field.name()|var_name }}
        {%- match field.default_value() %}
        {%- when Some with (literal) %} = {{ literal|literal_cpp(field) }};{%- else -%};
        {%- endmatch %}
        {%- endfor %}
    };
    {% endfor %}


    {% for variant in e.variants() %}
    {{ type_name }}({{ variant|variant_name }} variant): variant(variant) {}
    {%- endfor %}

    {{ type_name }}(const {{ type_name }} &other): variant(other.variant) {}
    {{ type_name }}({{ type_name }} &&other): variant(std::move(other.variant)) {}

    {{ type_name }} &operator=(const {{ type_name }} &other) {
        variant = other.variant;
        return *this;
    }

    {{ type_name }} &operator=({{ type_name }} &&other) {
        variant = std::move(other.variant);
        return *this;
    }

private:
    std::variant<{% for variant in e.variants() -%} {{ variant|variant_name }} {%- if !loop.last %}, {% endif -%} {% endfor %}> variant;

    {{ type_name }}();
};
{% endif %}