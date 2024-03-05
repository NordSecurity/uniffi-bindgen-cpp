{%- let type_name = typ|type_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let class_name = ffi_converter_name|class_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let iface = ci|get_callback_interface_definition(name) %}
std::once_flag {{ class_name }}::once = std::once_flag();
uniffi::HandleMap<{{ canonical_type_name }}> {{ class_name }}::callbacks = {};

{{ type_name }} {{ class_name }}::lift(uint64_t handle) {
    std::call_once({{ class_name }}::once, []() {
        rust_call({{ iface.ffi_init_callback().name() }}, nullptr, callback_stub);
    });

    return callbacks.at(handle);
}

uint64_t {{ class_name }}::lower(const {{type_name}}& impl) {
    std::call_once({{ class_name }}::once, []() {
        rust_call({{ iface.ffi_init_callback().name() }}, nullptr, callback_stub);
    });

    return callbacks.insert(impl);
}

{{ type_name }} uniffi::{{ class_name }}::read(RustStream &stream) {
    uint64_t handle;
    stream >> handle;

    return lift(handle);
}

void uniffi::{{ class_name }}::write(RustStream &stream, const {{ type_name }} &impl) {
    stream << lower(impl);
}

int32_t uniffi::{{ class_name }}::allocation_size(const {{ type_name }} &impl) {
    return static_cast<int32_t>(sizeof(uint64_t));
}

int {{ class_name }}::callback_stub(uint64_t handle, uint32_t method, uint8_t *args_data, int32_t args_len, RustBuffer *buf_ptr) {
    auto buf = RustBuffer {
        .capacity = args_len,
        .len = args_len,
        .data = args_data,
    };
    auto stream = RustStream(&buf);

    switch (method) {
    case 0:
        callbacks.erase(handle);
        break;
    {%- for method in iface.methods() %}
    case {{ loop.index }}: {
        {% if method.throws_type().is_some() %}{{ "try {" }}{% endif %}
            auto impl = lift(handle);
            {%- for arg in method.arguments() %}
            auto arg{{ loop.index0 }} = {{- arg|read_fn }}(stream);
            {%- endfor -%}
            {%- if method.return_type().is_some() %}
            auto ret = {% endif -%}
            impl->{{ method.name() }}(
            {%- for arg in method.arguments() %}
            std::move(arg{{ loop.index0 }}){%- if !loop.last %}, {% else %}{% endif %}
            {%- endfor -%}
        );
        {%- match method.return_type() %}
        {% when Some with (return_type) %}
            *buf_ptr = rustbuffer_alloc({{ return_type|allocation_size_fn }}(ret));
            auto out_stream = RustStream(buf_ptr);
            {{ return_type|write_fn }}(out_stream, ret);
        {%- else %}
        {%- endmatch %}
        {%- match method.throws_type() %}
        {% when Some with (e) %}
        } catch (const {{ e|type_name }} &ex) {
            *buf_ptr = {{ e|ffi_converter_name }}::lower(ex);
            return 1;
        } catch (std::exception &ex) {
            *buf_ptr = {{ Type::String.borrow()|lower_fn }}(ex.what());
            return 2;
        }
        {% else %}
        {%- endmatch %}
        break;
    }
    {%- endfor %}
    }

    return 0;
}
