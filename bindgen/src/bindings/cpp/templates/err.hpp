{%- let class_name = typ|type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name|class_name %}

namespace uniffi {
    struct {{ ffi_converter_name }};
}

struct {{ class_name }}: std::runtime_error {
    friend uniffi::{{ ffi_converter_name }};

    {{ class_name }}(const std::string &what_arg) : std::runtime_error(what_arg) {}

    virtual void throw_underlying() = 0;

protected:
    virtual int32_t get_variant_idx() const {
        return 0;
    };
};

{% for variant in e.variants() %}
struct {{ variant.name()|class_name }}: {{ class_name }} {
    {{ variant.name() }}(const std::string &what_arg) : {{ class_name }}(what_arg) {}

    void throw_underlying() override {
        throw *this;
    }

    int32_t get_variant_idx() const override {
        return {{ loop.index }};
    }
};
{% endfor %}
