{%- let type_name = typ|type_name %}
{{ type_name }} {{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto val = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return val;
}

RustBuffer {{ ffi_converter_name }}::lower(const {{ type_name }} &val) {
    auto buf = rustbuffer_alloc(allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return std::move(buf);
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    int64_t secs;
    uint32_t nanos;

    stream >> secs;
    stream >> nanos;

    auto sign = secs < 0 ? -1 : 1;

    auto duration = std::chrono::seconds(secs) + (sign * std::chrono::nanoseconds(nanos));

    return {{ type_name }}(duration);
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    auto duration = val.time_since_epoch();
    auto secs =  std::chrono::duration_cast<std::chrono::duration<int64_t>>(duration);
    auto nanos = (duration - secs).count();

    auto sign = secs.count() < 0 ? -1 : 1;

    stream << secs.count() << static_cast<uint32_t>(sign * nanos);
}

int32_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &) {
    return sizeof(int64_t) + sizeof(uint32_t);
}