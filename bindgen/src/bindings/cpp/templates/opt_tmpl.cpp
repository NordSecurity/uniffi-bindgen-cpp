{{ type_name }} {{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return ret;
}

RustBuffer {{ ffi_converter_name }}::lower(const {{ type_name }}& val) {
    auto buf = rustbuffer_alloc({{ ffi_converter_name }}::allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return buf;
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    char has_value;

    stream.get(has_value);

    {%- if typ|can_dereference_optional() %}
    if (has_value) {
        return {{ inner_type|read_fn }}(stream);
    } else {
        return nullptr;
    }
    {%- else %}
    if (has_value) {
        return std::make_optional({{ inner_type|read_fn }}(stream));
    } else {
        return std::nullopt;
    }
    {%- endif %}
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }}& value) {
    stream.put(static_cast<uint8_t>(!!value));

    if (value) {
        {%- if typ|can_dereference_optional() %}
        {{ inner_type|write_fn }}(stream, value);
        {%- else %}
        {{ inner_type|write_fn }}(stream, value.value());
        {%- endif %}
    }
}

int32_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &val) {
    int32_t ret = 1;

    if (val) {
        {%- if typ|can_dereference_optional() %}
        ret += {{ inner_type|allocation_size_fn }}(val);
        {%- else %}
        ret += {{ inner_type|allocation_size_fn }}(val.value());
        {%- endif %}
    }

    return ret;
}
