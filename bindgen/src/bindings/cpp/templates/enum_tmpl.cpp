{%- if e.is_flat() %}
{{ type_name }} uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return std::move(ret);
}

RustBuffer uniffi::{{ ffi_converter_name }}::lower(const {{ type_name }} &val) {
    auto buf = rustbuffer_alloc({{ ffi_converter_name }}::allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return std::move(buf);
}

{{ type_name }} uniffi::{{ ffi_converter_name }}::read(RustStream &stream) {
    int32_t variant;
    stream >> variant;

    switch (variant) {
        {% for variant in e.variants() %}
    case {{ loop.index }}:
        return {{ type_name }}::{{ variant|variant_name }};
        {% endfor %}
    default:
        throw std::runtime_error("No matching {{ type_name }} variant");
    }
}

void uniffi::{{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    stream << static_cast<int32_t>(val);
}

int32_t uniffi::{{ ffi_converter_name }}::allocation_size(const {{ type_name|class_name }} &) {
    return sizeof(int32_t);
}
{%- else %}
{{ type_name }} uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return std::move(ret);
}

RustBuffer uniffi::{{ ffi_converter_name }}::lower(const {{ type_name }} &val) {
    auto buf = rustbuffer_alloc({{ ffi_converter_name }}::allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return std::move(buf);
}

{{ type_name }} uniffi::{{ ffi_converter_name }}::read(RustStream &stream) {
    int32_t variant_id;
    stream >> variant_id;
    
    switch (variant_id) {
        {% for variant in e.variants() %}
    case {{ loop.index }}:
        return {{ type_name }}::{{ variant|variant_name }} {
            {%- for field in variant.fields() %}
            .{{field.name()|var_name}} = uniffi::{{ field|read_fn }}(stream),
            {%- endfor %}
        };
        {% endfor %}
    default:
        throw std::runtime_error("No matching {{ type_name }} variant");
    }
}

void uniffi::{{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    int32_t variant_id = val.variant.index() + 1;

    stream << variant_id;

    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        {%- for variant in e.variants() %}
        {% if !loop.first %}else {% endif %}if constexpr (std::is_same_v<T, {{ type_name }}::{{ variant|variant_name }}>) {
            {%- for field in variant.fields() %}
            uniffi::{{ field|write_fn }}(stream, arg.{{ field.name()|var_name }});
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

int32_t uniffi::{{ ffi_converter_name }}::allocation_size(const {{ type_name|class_name }} &val) {
    int32_t size = sizeof(int32_t);

    size += std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        {%- for variant in e.variants() %}
        {% if !loop.first %}else {% endif %}if constexpr (std::is_same_v<T, {{ type_name }}::{{ variant|variant_name }}>) {
            int32_t size = 0;
            {%- for field in variant.fields() %}
            size += uniffi::{{ field|allocation_size_fn }}(arg.{{ field.name()|var_name }});
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
{%- endif %}