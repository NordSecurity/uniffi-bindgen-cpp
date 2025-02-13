{%- let class_name = typ|canonical_name %}
{{ type_name }} {{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return ret;
}

RustBuffer {{ ffi_converter_name }}::lower(const {{ class_name }} &val) {
    auto buf = rustbuffer_alloc(allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return std::move(buf);
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    int32_t v;
    stream >> v;

    switch (v) {
    {%- for variant in e.variants() %}
    {%- if e.is_flat() %}
    case {{ loop.index }}:
        return std::make_shared<{{ class_name|to_lower_snake_case }}::{{ variant.name()|class_name }}>({{ Type::String.borrow()|read_fn }}(stream));
    {% else %}
    case {{ loop.index }}:
    {
        {{ class_name|to_lower_snake_case }}::{{ variant.name() }} var;
        {%- for field in variant.fields() %}
        var.{% call macros::field_name(field, loop.index) %} = {{ field|read_fn }}(stream);
        {%- endfor %}
        return std::make_shared<{{ class_name|to_lower_snake_case }}::{{ variant.name() }}>(var);
    }
    {%- endif %}
    {%- endfor %}
    default:
        throw std::runtime_error("Unexpected error variant");
    }
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ class_name }} &val) {
    stream << val.get_variant_idx();

    {%- if e.is_flat() %}
    {{ Type::String.borrow()|write_fn }}(stream, val.what());
    {%- else %}
    switch (val.get_variant_idx()) {
    {%- for variant in e.variants() %}
    case {{ loop.index }}:
    {
        auto var = static_cast<const {{ class_name|to_lower_snake_case }}::{{ variant.name() }}&>(val);
        {%- for field in variant.fields() %}
        {{ field|write_fn }}(stream, {{ field.as_type()|deref(ci) }}var.{% call macros::field_name(field, loop.index) %});
        {%- endfor %}
        break;
    }
    {%- endfor %}
    }
    {%- endif %}
}

uint64_t {{ ffi_converter_name }}::allocation_size(const {{ class_name }} &val) {
    {%- if e.is_flat() %}
    return static_cast<uint64_t>(sizeof(int32_t));
    {%- else %}
    switch (val.get_variant_idx()) {
    {%- for variant in e.variants() %}
    case {{ loop.index }}:
    {
        auto var = static_cast<const {{ class_name|to_lower_snake_case }}::{{ variant.name() }}&>(val);
        return static_cast<uint64_t>(sizeof(int32_t)
        {%- for field in variant.fields() %}
            + {{ field|allocation_size_fn }}({{ field.as_type()|deref(ci) }}var.{% call macros::field_name(field, loop.index) %})
        {%- endfor %});
    }
    {%- endfor %}
    default:
        throw std::runtime_error("Unexpected error variant");
    }
    {%- endif %}
}
