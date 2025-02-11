{%- let ffi_converter_name = typ|ffi_converter_name|class_name %}
{%- if e.is_flat() %}
{% call macros::docstring(e, 0) %}
enum class {{ type_name }}: int32_t {
    {%- for variant in e.variants() %}
    {%- call macros::docstring(variant, 4) %}
    {{ variant|variant_name(config.enum_style) }} = {{ loop.index }}
    {%- if !loop.last %},
    {%- endif %}
    {%- endfor %}
};
{%- else %}
namespace uniffi {
struct {{ ffi_converter_name }};
} // namespace uniffi

{%~ call macros::docstring(e, 0) %}
struct {{ type_name }} {
    friend uniffi::{{ ffi_converter_name }};

    {%- for variant in e.variants() %}
    {%- call macros::docstring(variant, 4) %}
    struct {{ variant|variant_name(config.enum_style) }} {
        {%- for field in variant.fields() %}
        {%- call macros::docstring(field, 8) %}
        {{ field|type_name(ci) }} {{ field.name()|var_name }}
        {%- match field.default_value() %}
        {%- when Some with (literal) %} = {{ literal|literal_cpp(field, config.enum_style, ci) }};{%- else -%};
        {%- endmatch %}
        {%- endfor %}
    };
    {%- endfor %}

    {%- for variant in e.variants() %}
    {{ type_name }}({{ variant|variant_name(config.enum_style) }} variant): variant(variant) {}
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

    /**
     * Returns the variant of this enum
     */
    const std::variant<{% for variant in e.variants() %}{{ variant|variant_name(config.enum_style) }}{% if !loop.last %}, {% endif %}{% endfor %}> &get_variant() const {
        return variant;
    }

private:
    std::variant<{% for variant in e.variants() %}{{ variant|variant_name(config.enum_style) }}{% if !loop.last %}, {% endif %}{% endfor %}> variant;

    {{ type_name }}();
};
{%- endif %}
