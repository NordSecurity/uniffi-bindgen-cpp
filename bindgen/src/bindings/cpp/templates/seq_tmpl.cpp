{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let class_name = ffi_converter_name|class_name %}
{%- let type_name = typ|type_name %}
{{ type_name }} {{ class_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = read(stream);

    rustbuffer_free(buf);

    return ret;
}

RustBuffer {{ class_name }}::lower(const {{ type_name }} &val) {
    auto buf = rustbuffer_alloc(allocation_size(val));
    auto stream = RustStream(&buf);

    write(stream, val);

    return buf;
}

{{ type_name }} {{ class_name }}::read(RustStream &stream) {
    {{ type_name }} ret;
    int32_t count;
    stream >> count;

    ret.reserve(count);

    for (decltype(count) i = 0; i < count; i++) {
        ret.push_back({{ inner_type|read_fn }}(stream));
    }

    return ret;
}

void {{ class_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    stream << static_cast<int32_t>(val.size());

    for (auto &elem : val) {
        {{ inner_type|write_fn }}(stream, elem);
    }
}

int32_t {{ class_name }}::allocation_size(const {{ type_name }} &val) {
    int32_t size = sizeof(int32_t);

    for (auto &elem : val) {
        size += {{ inner_type|allocation_size_fn }}(elem);
    }

    return size;
}
