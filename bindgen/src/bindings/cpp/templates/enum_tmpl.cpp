{%- if e.is_flat() %}
{{ type_name }} {{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return std::move(ret);
}

RustBuffer {{ ffi_converter_name }}::lower(const {{ type_name }} &val) {
    auto buf = rustbuffer_alloc({{ ffi_converter_name }}::allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return std::move(buf);
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    int32_t variant;
    stream >> variant;

    switch (variant) {
        {% for variant in e.variants() %}
    case {{ loop.index }}:
        return {{ type_name }}::{{ variant|variant_name(config.enum_style) }};
        {% endfor %}
    default:
        throw std::runtime_error("No matching {{ type_name }} variant");
    }
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    switch (val) {
        {% for variant in e.variants() %}
    case {{ type_name }}::{{ variant|variant_name(config.enum_style) }}:
        stream << static_cast<int32_t>({{ loop.index }});
        break;
        {% endfor %}
    default:
        throw std::runtime_error("No matching {{ type_name }} variant");
    }
}

uint64_t {{ ffi_converter_name }}::allocation_size(const {{ type_name|class_name }} &) {
    return static_cast<uint64_t>(sizeof(int32_t));
}
{%- else %}
{{ type_name }} {{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return std::move(ret);
}

RustBuffer {{ ffi_converter_name }}::lower(const {{ type_name }} &val) {
    auto buf = rustbuffer_alloc({{ ffi_converter_name }}::allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return std::move(buf);
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    int32_t variant_id;
    stream >> variant_id;

    switch (variant_id) {
        {% for variant in e.variants() %}
    case {{ loop.index }}:
        return {{ type_name }}::{{ variant|variant_name(config.enum_style) }} {
            {%- for field in variant.fields() %}
            .{{field.name()|var_name}} = {{ field|read_fn }}(stream),
            {%- endfor %}
        };
        {% endfor %}
    default:
        throw std::runtime_error("No matching {{ type_name }} variant");
    }
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    int32_t variant_id = static_cast<int32_t>(val.variant.index() + 1);

    stream << variant_id;

    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        {%- for variant in e.variants() %}
        {% if !loop.first %}else {% endif %}if constexpr (std::is_same_v<T, {{ type_name }}::{{ variant|variant_name(config.enum_style) }}>) {
            {%- for field in variant.fields() %}
            {{ field|write_fn }}(stream, {{ field.as_type()|deref(ci) }}arg.{{ field.name()|var_name }});
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

uint64_t {{ ffi_converter_name }}::allocation_size(const {{ type_name|class_name }} &val) {
    uint64_t size = sizeof(int32_t);

    size += std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        {%- for variant in e.variants() %}
        {% if !loop.first %}else {% endif %}if constexpr (std::is_same_v<T, {{ type_name }}::{{ variant|variant_name(config.enum_style) }}>) {
            uint64_t size = 0;
            {%- for field in variant.fields() %}
            size += {{ field|allocation_size_fn }}({{ field.as_type()|deref(ci) }}arg.{{ field.name()|var_name }});
            {%- endfor %}
            return size;
        }
        {%- endfor %}
        {%- if e.variants().len() != 0 %}
        else {
            static_assert(always_false_v<T>, "non-exhaustive {{ type_name }} visitor");
        }
        {%- endif %}
    }, val.variant);

    return size;
}
{%- endif %}
