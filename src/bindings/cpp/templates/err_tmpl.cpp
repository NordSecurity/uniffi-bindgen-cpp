struct {{ type_name|class_name }}: std::runtime_error {
    {{ type_name|class_name }}(const std::string &what_arg) : std::runtime_error(what_arg) {}
};
{% for variant in e.variants() %}
struct {{ variant.name()|class_name }}: {{ type_name }} {
    {{ variant.name() }}(const std::string &what_arg) : {{ type_name }}(what_arg) {}
{% if !e.is_flat() %}
{%- endif %}
};
{% endfor %}

struct {{ ffi_converter_name }} {
    static {{ type_name|class_name }} lift(RustBuffer buf) {
        auto stream = uniffi::RustStream(&buf);
        auto ret = {{ ffi_converter_name }}::read(stream);

        rustbuffer_free(buf);

        return ret;
    }

    static {{ type_name|class_name }} read(RustStream &stream) {
        int32_t v;

        stream >> v;

        switch (v) {
        {%- if e.is_flat() %}
        {%- for variant in e.variants() %}
        case {{ loop.index }}:
            return {{ variant.name()|class_name }}({{ Type::String.borrow()|read_fn }}(stream));
        {%- endfor %}
        default:
            throw std::runtime_error("Unexpected error variant");
        {%- endif %}
        }
    }
};
