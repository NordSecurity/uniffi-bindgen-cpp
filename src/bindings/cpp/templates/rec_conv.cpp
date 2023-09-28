{%- let rec = ci.get_record_definition(name).unwrap() %}
{%- let class_name = type_name|class_name %}

::{{ namespace }}::{{ class_name }} {{ ffi_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto ret = {{ ffi_converter_name }}::read(stream);

    rustbuffer_free(buf);

    return std::move(ret);
}

RustBuffer {{ ffi_converter_name }}::lower(const ::{{ namespace }}::{{ class_name }} &val) {
    auto buf = rustbuffer_alloc(allocation_size(val));
    auto stream = RustStream(&buf);

    {{ ffi_converter_name }}::write(stream, val);

    return std::move(buf);
}

::{{ namespace }}::{{ class_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    return {
        {%- for field in rec.fields() %}
        {{ field|read_fn }}(stream){% if !loop.last %},{% endif %}
        {%- endfor %}
    };
}

void {{ ffi_converter_name }}::write(RustStream &stream, const ::{{ namespace }}::{{ class_name }} &val) {
    {%- for field in rec.fields() %}
    {{ field|write_fn }}(stream, val.{{ field.name()|var_name }});
    {%- endfor %}
}

int32_t {{ ffi_converter_name }}::allocation_size(const ::{{ namespace }}::{{ class_name }} &val) {
    return {% for field in rec.fields() %}{{ field|allocation_size_fn}}(val.{{ field.name()|var_name() }})
        {% if !loop.last %}+ {% else %};{% endif %}
    {%- endfor %}
}
