{%- let rec = ci|get_record_definition(name) %}
{%- let class_name = type_name|class_name %}
{{ class_name }} {{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return std::move(ret);
}

RustBuffer {{ ffi_converter_name }}::lower(const {{ class_name }} &val) {
    auto buf = rustbuffer_alloc(allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return std::move(buf);
}

{{ class_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    return {
        {%- for field in rec.fields() %}
        {{ field|read_fn }}(stream){% if !loop.last %},{% endif %}
        {%- endfor %}
    };
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ class_name }} &val) {
    {%- for field in rec.fields() %}
    {{ field|write_fn }}(stream, val.{{ field.name()|var_name }});
    {%- endfor %}
}

int32_t {{ ffi_converter_name }}::allocation_size(const {{ class_name }} &val) {
    {% if rec.fields().is_empty() %}
    return 0;
    {% else %}
    return {% for field in rec.fields() %}
        {{ field|allocation_size_fn}}(val.{{ field.name()|var_name() }}){% if !loop.last %} +{% else -%};{%- endif %}
    {%- endfor %}
    {% endif %}
}
