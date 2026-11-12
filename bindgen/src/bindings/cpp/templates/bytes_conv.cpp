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

ForeignBytes {{ ffi_converter_name }}::borrow(const {{ type_name }} &val) {
    return ForeignBytes { static_cast<int32_t>(val.size()), const_cast<uint8_t *>(val.data()) };
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    {{ type_name }} ret;
    int32_t count;
    stream >> count;

    ret.reserve(count);

    for (decltype(count) i = 0; i < count; i++) {
        uint8_t elem;
        stream >> elem;
        ret.push_back(elem);
    }

    return ret;
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    stream << static_cast<int32_t>(val.size());

    for (auto &elem : val) {
        stream << elem;
    }
}

uint64_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &val) {
    return static_cast<uint64_t>(sizeof(int32_t) + sizeof(uint8_t) * val.size());
}
