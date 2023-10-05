namespace {{ namespace }} {
    {{ type_name }} {{ namespace }}::uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
        auto stream = {{ namespace }}::uniffi::RustStream(&buf);
        auto ret = {{ namespace }}::uniffi::{{ ffi_converter_name }}::read(stream);

        {{ namespace }}::uniffi::rustbuffer_free(buf);

        return ret;
    }

    RustBuffer {{ namespace }}::uniffi::{{ ffi_converter_name }}::lower({{ type_name }} val) {
        auto buf = {{ namespace }}::uniffi::rustbuffer_alloc({{ namespace }}::uniffi::{{ ffi_converter_name }}::allocation_size(val));
        auto stream = {{ namespace }}::uniffi::RustStream(&buf);

        {{ namespace }}::uniffi::{{ ffi_converter_name }}::write(stream, val);

        return buf;
    }

    {{ type_name }} {{ namespace }}::uniffi::{{ ffi_converter_name }}::read({{ namespace }}::uniffi::RustStream &stream) {
        uint8_t has_value;

        stream.get(has_value);

        if (has_value) {
            return std::make_optional({{ namespace }}::uniffi::{{ inner_type|read_fn }}(stream));
        }

        return std::nullopt;
    }

    void {{ namespace }}::uniffi::{{ ffi_converter_name }}::write({{ namespace }}::uniffi::RustStream &stream, {{ type_name }} value) {
        stream.put(static_cast<uint8_t>(!!value));

        if (value) {
            {{ namespace }}::uniffi::{{ inner_type|write_fn }}(stream, value.value());
        }
    }

    int32_t {{ namespace }}::uniffi::{{ ffi_converter_name }}::allocation_size(const {{ type_name }} &val) {
        int32_t ret = 1;

        if (val) {
            ret += {{ namespace }}::uniffi::{{ inner_type|allocation_size_fn }}(val.value());
        }

        return ret;
    }
}
