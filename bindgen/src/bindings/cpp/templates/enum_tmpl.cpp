{%- if e.is_flat() %}
namespace {{ namespace }} {
    {{ namespace }}::{{ type_name }} uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
        auto stream = ::{{ namespace }}::uniffi::RustStream(&buf);
        auto ret = {{ namespace }}::uniffi::{{ ffi_converter_name }}::read(stream);

        {{ namespace }}::uniffi::rustbuffer_free(buf);

        return std::move(ret);
    }

    RustBuffer uniffi::{{ ffi_converter_name }}::lower(const {{ namespace }}::{{ type_name }} &val) {
        auto buf = {{ namespace }}::uniffi::rustbuffer_alloc({{ namespace }}::uniffi::{{ ffi_converter_name }}::allocation_size(val));
        auto stream = {{ namespace }}::uniffi::RustStream(&buf);

        {{ namespace }}::uniffi::{{ ffi_converter_name }}::write(stream, val);

        return std::move(buf);
    }

    {{ type_name }} uniffi::{{ ffi_converter_name }}::read(::{{ namespace }}::uniffi::RustStream &stream) {
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

    void uniffi::{{ ffi_converter_name }}::write(uniffi::RustStream &stream, const {{ namespace }}::{{ type_name }} &val) {
        stream << static_cast<int32_t>(val);
    }

    int32_t uniffi::{{ ffi_converter_name }}::allocation_size(const {{ namespace }}::{{ type_name|class_name }} &) {
        return sizeof(int32_t);
    }
}
{%- else %}
namespace {{ namespace }} {
    {{ namespace }}::{{ type_name }} uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
        auto stream = ::{{ namespace }}::uniffi::RustStream(&buf);
        auto ret = {{ namespace }}::uniffi::{{ ffi_converter_name }}::read(stream);

        {{ namespace }}::uniffi::rustbuffer_free(buf);

        return std::move(ret);
    }

    RustBuffer uniffi::{{ ffi_converter_name }}::lower(const {{ namespace }}::{{ type_name }} &val) {
        auto buf = {{ namespace }}::uniffi::rustbuffer_alloc({{ namespace }}::uniffi::{{ ffi_converter_name }}::allocation_size(val));
        auto stream = {{ namespace }}::uniffi::RustStream(&buf);

        {{ namespace }}::uniffi::{{ ffi_converter_name }}::write(stream, val);

        return std::move(buf);
    }

    {{ type_name }} uniffi::{{ ffi_converter_name }}::read(::{{ namespace }}::uniffi::RustStream &stream) {
        int32_t variant_id;

        stream >> variant_id;
        
        switch (variant_id) {
            {%- for variant in e.variants() %}
        case {{ loop.index }}:
            return {{ type_name }}::{{ variant|variant_name }} {
                {%- for field in variant.fields() %}
                .{{field.name()|var_name}} = {{ field|read_fn }}(stream),
                {%- endfor %}
            };
            {%- endfor %}
        default:
            throw std::runtime_error("No matching {{ type_name }} variant");
        }
    }

    void uniffi::{{ ffi_converter_name }}::write(uniffi::RustStream &stream, const {{ namespace }}::{{ type_name }} &val) {
        int32_t variant_id = val.variant.index() + 1;

        stream << variant_id;

        std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            {% for variant in e.variants() %}
            {%- if !loop.first %} else {% endif -%} if constexpr (std::is_same_v<T, {{ type_name }}::{{ variant|variant_name }}>) {
                {%- for field in variant.fields() %}
                {{ field|write_fn }}(stream, arg.{{ field.name()|var_name }});
                {%- endfor %}
            }
            {%- endfor %}
            {%- if e.variants().len() != 0 %}
            else {
                static_assert(always_false_v<T>, "non-exhaustive {{ type_name }} visitor");
            }
            {%- endif %}
        }, val.variant); 
    }

    int32_t uniffi::{{ ffi_converter_name }}::allocation_size(const {{ namespace }}::{{ type_name|class_name }} &val) {
        int32_t size = sizeof(int32_t);

        size += std::visit([&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            {% for variant in e.variants() %}
            {%- if !loop.first %} else {% endif -%} if constexpr (std::is_same_v<T, {{ type_name }}::{{ variant|variant_name }}>) {
                int32_t size = 0;
                {%- for field in variant.fields() %}
                size += {{ field|allocation_size_fn }}(arg.{{ field.name()|var_name }});
                {%- endfor %}
                return size;
            }
            {%- endfor %}
            {%- if e.variants().len() != 0 %}
            else {
                static_assert(always_false_v<T>, "non-exhaustive {{ type_name }} visitor");
            }
            {%- endif %}

            return 0;
        }, val.variant);

        return size;
    }
}
{% endif %}
