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
        {%- if field|needs_smart_ptr_wrap(ci) %}
        std::make_shared<{{ field|extract_inner_type(ci) }}>({{ field|read_fn }}(stream)){% if !loop.last %},{% endif %}
        {%- else %}
        {{ field|read_fn }}(stream){% if !loop.last %},{% endif %}
        {%- endif %}
        {%- endfor %}
    };
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ class_name }} &val) {
    {%- for field in rec.fields() %}
    {%- if field|needs_smart_ptr_wrap(ci) %}
    {{ field|write_fn }}(stream, {{ field.as_type()|deref }}*val.{{ field.name()|var_name }});
    {%- else %}
    {{ field|write_fn }}(stream, {{ field.as_type()|deref }}val.{{ field.name()|var_name }});
    {%- endif %}
    {%- endfor %}
}

uint64_t {{ ffi_converter_name }}::allocation_size(const {{ class_name }} &val) {
    {% if rec.fields().is_empty() %}
    return 0;
    {% else %}
    return {% for field in rec.fields() %}
        {%- if field|needs_smart_ptr_wrap(ci) %}
        {{ field|allocation_size_fn}}({{ field.as_type()|deref }}*val.{{ field.name()|var_name() }}){% if !loop.last %} +{% else -%};{%- endif %}
        {%- else %}
        {{ field|allocation_size_fn}}({{ field.as_type()|deref }}val.{{ field.name()|var_name() }}){% if !loop.last %} +{% else -%};{%- endif %}
        {%- endif %}
    {%- endfor %}
    {% endif %}
}
