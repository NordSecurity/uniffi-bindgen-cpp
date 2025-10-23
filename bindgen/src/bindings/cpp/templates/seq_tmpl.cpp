{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let class_name = ffi_converter_name|class_name %}
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
        {%- if inner_type|vector_element_needs_wrapping(ci) %}
        ret.push_back(std::make_shared<{{ inner_type|type_name(ci) }}>({{ inner_type|read_fn }}(stream)));
        {%- else %}
        ret.push_back({{ inner_type|read_fn }}(stream));
        {%- endif %}
    }

    return ret;
}

void {{ class_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    stream << static_cast<int32_t>(val.size());

    for (auto &elem : val) {
        {%- if inner_type|vector_element_needs_wrapping(ci) %}
        {{ inner_type|write_fn }}(stream, {{ inner_type|deref }}*elem);
        {%- else %}
        {{ inner_type|write_fn }}(stream, {{ inner_type|deref }}elem);
        {%- endif %}
    }
}

uint64_t {{ class_name }}::allocation_size(const {{ type_name }} &val) {
    uint64_t size = sizeof(int32_t);

    for (auto &elem : val) {
        {%- if inner_type|vector_element_needs_wrapping(ci) %}
        size += {{ inner_type|allocation_size_fn }}({{inner_type|deref }}*elem);
        {%- else %}
        size += {{ inner_type|allocation_size_fn }}({{inner_type|deref }}elem);
        {%- endif %}
    }

    return size;
}
