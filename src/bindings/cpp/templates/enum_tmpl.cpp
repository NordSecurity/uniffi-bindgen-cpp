{%- if e.is_flat() %}
enum class {{ type_name }}: int32_t {
    {% for variant in e.variants() -%}
    {{ variant|variant_name }} = {{ loop.index }}
    {%- if !loop.last %},
    {% endif -%}
    {% endfor %}
};

struct {{ ffi_converter_name }} {
    static {{ type_name }} lift(RustBuffer buf) {
        auto stream = ::{{ namespace }}::uniffi::RustBuffer(&buf);
        auto ret = {{ ffi_converter_name }}::read(stream);

        ::{{ namespace }}::uniffi::rustbuffer_free(buf);

        return std::move(ret);
    }

    static RustBuffer lower({{ type_name }} val) {
        auto buf = rustbuffer_alloc(allocation_size(val));
        auto stream = ::{{ namespace }}::uniffi::RustStream(&buf);

        {{ ffi_converter_name }}::write(stream, val);

        return std::move(buf);
    }

    static {{ type_name }} read(::{{ namespace }}::uniffi::RustStream &stream) {
        int32_t variant;

        stream >> variant;

        switch (variant) {
            {%- for variant in e.variants() %}
        case {{ loop.index }}:
            return {{ type_name }}::{{ variant|variant_name }};
            {%- endfor %}
        default:
            throw std::runtime_error("No matching {{ type_name }} variant");
        }
    }

    static void write(uint8_t *buf, {{ type_name }} val) {
        auto ser = static_cast<int32_t>(val);

        if (std::endian::native != std::endian::big) {
            ser = std::byteswap(ser);
        }

        auto bytes = std::bit_cast<uint8_t *>(&ser);

        std::copy(bytes, bytes + sizeof({{ type_name }}), buf);
    }

    static int32_t allocation_size(const {{ type_name|class_name }} &) {
        return sizeof({{ type_name|class_name }});
    }
};
{% else %}
struct {{ type_name }} {
    {% for variant in e.variants() -%}
    class {{ variant|variant_name }} {
    };
    {% endfor %}
private:
    {{ type_name }}() = delete;
    {{ type_name }}(const {{ type_name }} &) = delete;
    {{ type_name }}({{ type_name }} &&) = delete;
    {{ type_name }} &operator=(const {{ type_name }} &) = delete;
    {{ type_name }} &operator=({{ type_name }} &&) = delete;
};
{% endif %}
