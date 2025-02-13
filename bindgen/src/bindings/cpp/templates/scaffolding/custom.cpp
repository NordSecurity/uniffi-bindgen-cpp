{%- let converter = typ|ffi_converter_name -%}
{%- let custom_converter = "UniffiCustomTypeConverter{}"|format(typ|type_name(ci)) -%}
{{ type_name }} {{ converter }}::lift(RustBuffer buff) {
    auto stream = RustStream(&buff);
    auto builtin_val = {{ builtin|read_fn }}(stream);

    return {{ custom_converter }}::into_custom(builtin_val);
}

RustBuffer {{ converter }}::lower(const {{ type_name }} &val) {
    auto buf = rustbuffer_alloc(allocation_size(val));
    auto stream = RustStream(&buf);
    auto builtin_val = {{ custom_converter }}::from_custom(val);

    {{ builtin|write_fn }}(stream, builtin_val);

    return std::move(buf);
}

{{ type_name }} {{ converter }}::read(RustStream &stream) {
    auto builtin_val = {{ builtin|read_fn }}(stream);

    return {{ custom_converter }}::into_custom(builtin_val);
}

void {{ converter }}::write(RustStream &stream, const {{ type_name }} &val) {
    auto builtin_val = {{ custom_converter }}::from_custom(val);

    {{ builtin|write_fn }}(stream, builtin_val);
}

int32_t {{ converter }}::allocation_size(const {{ type_name }} &val) {
    auto builtin_val = {{ custom_converter }}::from_custom(val);

    return {{ builtin|allocation_size_fn }}(builtin_val);
}

