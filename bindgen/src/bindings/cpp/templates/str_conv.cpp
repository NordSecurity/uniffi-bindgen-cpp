std::string {{ namespace }}::uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto string = std::string(reinterpret_cast<char *>(buf.data), buf.len);

    {{ namespace }}::uniffi::rustbuffer_free(buf);

    return string;
}

RustBuffer {{ namespace }}::uniffi::{{ ffi_converter_name }}::lower(const std::string &val) {
    auto len = static_cast<int32_t>(val.length());
    auto bytes = ForeignBytes { len, reinterpret_cast<uint8_t *>(const_cast<char *>(val.data())) };

    return {{ namespace }}::uniffi::rustbuffer_from_bytes(bytes);
}

std::string {{ namespace }}::uniffi::{{ ffi_converter_name }}::read({{ namespace }}::uniffi::RustStream &stream) {
    int32_t len;
    std::string string;

    stream >> len;

    string.resize(len);
    stream.read(reinterpret_cast<uint8_t *>(string.data()), len);

    return string;
}

void {{ namespace }}::uniffi::{{ ffi_converter_name }}::write({{ namespace }}::uniffi::RustStream &stream, const std::string &val) {
    stream << static_cast<int32_t>(val.length());
    stream.write(reinterpret_cast<const uint8_t *>(val.data()), val.length());
}

int32_t {{ namespace }}::uniffi::{{ ffi_converter_name }}::allocation_size(const std::string &val) {
    return sizeof(int32_t) + val.length();
}
