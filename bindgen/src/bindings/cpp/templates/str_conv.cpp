std::string {{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto string = std::string(reinterpret_cast<char *>(buf.data), buf.len);

    rustbuffer_free(buf);

    return string;
}

RustBuffer {{ ffi_converter_name }}::lower(const std::string &val) {
    auto len = static_cast<int32_t>(val.length());
    auto bytes = ForeignBytes { len, reinterpret_cast<uint8_t *>(const_cast<char *>(val.data())) };

    return rustbuffer_from_bytes(bytes);
}

std::string {{ ffi_converter_name }}::read(RustStream &stream) {
    int32_t len;
    std::string string;

    stream >> len;

    string.resize(len);
    stream.read(string.data(), len);

    return string;
}

void {{ ffi_converter_name }}::write(RustStream &stream, const std::string &val) {
    stream << static_cast<int32_t>(val.length());
    stream.write(val.data(), val.length());
}

int32_t {{ ffi_converter_name }}::allocation_size(const std::string &val) {
    return static_cast<int32_t>(sizeof(int32_t) + val.length());
}
