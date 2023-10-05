{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

namespace {{ namespace }} {
    {{ namespace }}::{{ class_name }} {{ namespace }}::uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
        auto stream = {{ namespace }}::uniffi::RustStream(&buf);
        auto ret = {{ ffi_converter_name }}::read(stream);

        {{ namespace }}::uniffi::rustbuffer_free(buf);

        return ret;
    }

    {{ namespace }}::{{ class_name }} {{ namespace }}::uniffi::{{ ffi_converter_name }}::read({{ namespace }}::uniffi::RustStream &stream) {
        int32_t v;

        stream >> v;

        switch (v) {
        {%- if e.is_flat() %}
        {%- for variant in e.variants() %}
        case {{ loop.index }}:
            return {{ variant.name()|class_name }}({{ namespace }}::uniffi::{{ Type::String.borrow()|read_fn }}(stream));
        {%- endfor %}
        default:
            throw std::runtime_error("Unexpected error variant");
        {%- endif %}
        }
    }
}
