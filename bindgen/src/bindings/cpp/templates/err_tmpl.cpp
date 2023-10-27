{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

namespace {{ namespace }} {
    std::unique_ptr<{{ class_name }}> uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
        auto stream = uniffi::RustStream(&buf);
        auto ret = {{ ffi_converter_name }}::read(stream);

        uniffi::rustbuffer_free(buf);

        return ret;
    }

    RustBuffer uniffi::{{ ffi_converter_name }}::lower(const {{ class_name }} &val) {
        auto buf = rustbuffer_alloc(allocation_size(val));
        auto stream = uniffi::RustStream(&buf);

        {{ ffi_converter_name }}::write(stream, val);

        return std::move(buf);
    }

    std::unique_ptr<{{ class_name }}> uniffi::{{ ffi_converter_name }}::read(uniffi::RustStream &stream) {
        int32_t v;

        stream >> v;

        switch (v) {
        {%- if e.is_flat() %}
        {%- for variant in e.variants() %}
        case {{ loop.index }}:
            return std::make_unique<{{ variant.name()|class_name }}>(uniffi::{{ Type::String.borrow()|read_fn }}(stream));
        {%- endfor %}
        default:
            throw std::runtime_error("Unexpected error variant");
        {%- endif %}
        }
    }

    void uniffi::{{ ffi_converter_name }}::write(uniffi::RustStream &stream, const {{ class_name }} &val) {
    }

    int32_t uniffi::{{ ffi_converter_name }}::allocation_size(const {{ class_name }} &val) {
        {% if e.is_flat() -%}
        return sizeof(int32_t);
        {% else %}
        switch (val.get_variant_idx()) {
        {% for variant in e.variants() %}
        case {{ loop.index }}:
            return sizeof(int32_t)
            {%- for field in variant.fields() %}
                + {{ field|allocation_size_fn }}(val.{{ field.name()|var_name }})
            {%- endfor %};
        {% endfor %}
        }
        {%- endif %}
    }
}
