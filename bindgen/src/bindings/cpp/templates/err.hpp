{%- let class_name = typ|canonical_name %}
{%- let ffi_converter_name = typ|ffi_converter_name|class_name %}
namespace uniffi {
struct {{ ffi_converter_name }};
} // namespace uniffi

{%~ call macros::docstring(e, 0) %}
struct {{ class_name }}: std::runtime_error {
    friend uniffi::{{ ffi_converter_name }};

    {{ class_name }}() : std::runtime_error("") {}
    {{ class_name }}(const std::string &what_arg) : std::runtime_error(what_arg) {}

    virtual ~{{ class_name }}() = default;

    virtual void throw_underlying() {
        throw *this;
    }

protected:
    virtual int32_t get_variant_idx() const {
        return 0;
    };
};

{%- if e.variants().len() != 0 %}
/**
 * Contains variants of {{ class_name }}
 */
namespace {{ class_name|to_lower_snake_case }} {
{%- for variant in e.variants() %}
{% call macros::docstring(variant, 0) %}
struct {{ variant.name()|class_name }}: {{ class_name }} {
    {%- for field in variant.fields() %}
    {{ field|type_name(ci) }} {% call macros::field_name(field, loop.index) %}
    {%- match field.default_value() %}
    {% when Some with (literal) %} = {{ literal|literal_cpp(field, config.enum_style, ci) }};{% else %};
    {%- endmatch %}
    {%- endfor %}

    {{ variant.name()|class_name }}() : {{ class_name }}("") {}
    {{ variant.name()|class_name }}(const std::string &what_arg) : {{ class_name }}(what_arg) {}

    void throw_underlying() override {
        throw *this;
    }

protected:
    int32_t get_variant_idx() const override {
        return {{ loop.index }};
    }
};
{%- endfor %}
} // namespace {{ class_name|to_lower_snake_case }}
{%- endif %}
