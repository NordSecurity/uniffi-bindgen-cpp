{{ type_name }} {{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = read(stream);

    rustbuffer_free(buf);

    return ret;
}

RustBuffer {{ ffi_converter_name }}::lower(const {{ type_name }} &val) {
    auto buf = rustbuffer_alloc(allocation_size(val));
    auto stream = RustStream(&buf);

    write(stream, val);

    return buf;
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    int32_t count;
    stream >> count;

    {{ type_name }} ret(static_cast<size_t>(count));
    if (count != 0) {
        stream.read(reinterpret_cast<char *>(ret.data()),
                    static_cast<std::streamsize>(count));
    }

    return ret;
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    stream << static_cast<int32_t>(val.size());

    if (!val.empty()) {
        stream.write(reinterpret_cast<const char *>(val.data()),
                     static_cast<std::streamsize>(val.size()));
    }
}

uint64_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &val) {
    return static_cast<uint64_t>(sizeof(int32_t) + sizeof(uint8_t) * val.size());
}
