{%- let type_name = e|type_name(ci) %}
{%- let class_name = type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let namespace = type_name|to_lower_snake_case %}

{{ namespace }}::{{ variant.name() }} {{ ffi_converter_name }}{{ variant.name() }}::lift(RustBuffer buf, int32_t v) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}{{ variant.name() }}::read(stream, v);

    rustbuffer_free(buf);

    return ret;
}

RustBuffer {{ ffi_converter_name }}{{ variant.name() }}::lower(const {{ namespace }}::{{ variant.name() }} &val) {
    auto buf = rustbuffer_alloc(allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}{{ variant.name() }}::write(stream, val);

    return std::move(buf);
}

{{ namespace }}::{{ variant.name() }} {{ ffi_converter_name }}{{ variant.name() }}::read(RustStream &stream, int32_t v) {
    if (v != {{ loop.index }}) {
        throw std::runtime_error("Unexpected error variant");
    }

    {%- if e.is_flat() %}
    return {{ namespace }}::{{ variant.name() }}();
    {%- else %}
    {{ namespace }}::{{ variant.name() }} var;
    {%- for field in variant.fields() %}
    var.{{ field.name()|var_name }} = {{ field|read_fn }}(stream);
    {%- endfor %}
    return var;
    {%- endif %}
}

void {{ ffi_converter_name }}{{ variant.name() }}::write(RustStream &stream, const {{ namespace }}::{{ variant.name() }} &val) {
    stream << int32_t({{ loop.index }});

    {%- if e.is_flat() %}
    {{ Type::String.borrow()|write_fn }}(stream, val.what());
    {%- else %}
    {%- for field in variant.fields() %}
    {{ field|write_fn }}(stream, val.{{ field.name()|var_name }});
    {%- endfor %}
    {%- endif %}
}

int32_t {{ ffi_converter_name }}{{ variant.name() }}::allocation_size(const {{ namespace }}::{{ variant.name() }} &val) {
    int32_t size = sizeof(int32_t);
    {%- if e.is_flat() %}
    size += {{ Type::String.borrow()|allocation_size_fn }}(val.what());
    {%- else %}
    {%- for field in variant.fields() %}
    size += {{ field|allocation_size_fn }}(val.{{ field.name()|var_name }});
    {%- endfor %}
    {%- endif %}

    return size;
}
