{%- match config.custom_types.get(name.as_str()) %}
{%- when Some with (type_config) %}
{%- match type_config.type_name %}
{%- when Some with (type_name) %}
{%- let ffi_type_name = builtin|ffi_type|ffi_type_name %}
{{ type_name }} {{ ffi_converter_name }}::lift(RustBuffer buff) {
    auto builtin_val = {{ builtin|lift_fn }}(buff);

    return {{ type_config.into_custom.render("builtin_val") }};
}

RustBuffer {{ ffi_converter_name }}::lower(const {{ type_name }} &val) {
    auto builtin_val = {{ type_config.from_custom.render("val") }};

    return {{ builtin|lower_fn }}(builtin_val);
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    auto builtin_val = {{ builtin|read_fn }}(stream);

    return {{ type_config.into_custom.render("builtin_val") }};
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &val) {
    auto builtin_val = {{ type_config.from_custom.render("val") }};

    {{ builtin|write_fn }}(stream, builtin_val);
}

int32_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &val) {
    auto builtin_val = {{ type_config.from_custom.render("val") }};

    return {{ builtin|allocation_size_fn }}(builtin_val);
}
{%- else %}
{%- endmatch %}
{%- else %}
{%- endmatch %}
