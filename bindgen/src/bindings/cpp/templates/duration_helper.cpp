{%- let type_name = typ|type_name %}

{{ type_name }} {{ namespace }}::uniffi::{{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = uniffi::RustStream(&buf);
    auto val = {{ ffi_converter_name }}::read(stream);

    uniffi::rustbuffer_free(buf);

    return val;
}

RustBuffer {{ namespace }}::uniffi::{{ ffi_converter_name }}::lower(const {{ type_name }} &val) {
    auto buf = rustbuffer_alloc(allocation_size(val));
    auto stream = uniffi::RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return std::move(buf);
}

{{ type_name }} {{ namespace }}::uniffi::{{ ffi_converter_name }}::read({{ namespace }}::uniffi::RustStream &stream) {
    uint64_t secs;
    uint32_t nanos;

    stream >> secs;
    stream >> nanos;

    return std::chrono::seconds(secs) + std::chrono::nanoseconds(nanos);
}

void {{ namespace }}::uniffi::{{ ffi_converter_name }}::write({{ namespace }}::uniffi::RustStream &stream, const {{ type_name }} &val) {
    auto secs =  std::chrono::duration_cast<std::chrono::seconds>(val);
    auto nanos = (val - secs).count();

    stream << secs.count() << static_cast<uint32_t>(nanos);
}

int32_t {{ namespace }}::uniffi::{{ ffi_converter_name }}::allocation_size(const {{ type_name }} &) {
    return sizeof(uint64_t) + sizeof(uint32_t);
}
