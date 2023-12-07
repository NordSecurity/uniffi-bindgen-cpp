{{ type_name }} uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return ret;
}

RustBuffer uniffi::{{ ffi_converter_name }}::lower(const {{ type_name }}& val) {
    auto buf = rustbuffer_alloc({{ ffi_converter_name }}::allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return buf;
}

{{ type_name }} uniffi::{{ ffi_converter_name }}::read(RustStream &stream) {
    uint8_t has_value;

    stream.get(has_value);

    if (has_value) {
        return std::make_optional({{ inner_type|read_fn }}(stream));
    }

    return std::nullopt;
}

void uniffi::{{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }}& value) {
    stream.put(static_cast<uint8_t>(!!value));

    if (value) {
        {{ inner_type|write_fn }}(stream, value.value());
    }
}

int32_t uniffi::{{ ffi_converter_name }}::allocation_size(const {{ type_name }} &val) {
    int32_t ret = 1;

    if (val) {
        ret += {{ inner_type|allocation_size_fn }}(val.value());
    }

    return ret;
}