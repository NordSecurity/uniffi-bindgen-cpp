{%- let type_name = typ|type_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let class_name = ffi_converter_name|class_name %}
{%- let iface = ci|get_callback_interface_definition(name) %}

namespace {{ namespace }} {
    std::once_flag uniffi::{{ class_name }}::once = std::once_flag();
    uniffi::HandleMap<{{ type_name|class_name }}> uniffi::{{ class_name }}::callbacks = {};

    std::shared_ptr<{{ type_name|class_name }}> uniffi::{{ class_name }}::lift(uint64_t handle) {
        std::call_once(uniffi::{{ class_name }}::once, []() {
            uniffi::rust_call({{ iface.ffi_init_callback().name() }}, nullptr, callback_stub);
        });

        return callbacks.at(handle);
    }

    uint64_t uniffi::{{ class_name }}::lower(std::shared_ptr<{{ type_name|class_name }}> impl) {
        std::call_once(uniffi::{{ class_name }}::once, []() {
            uniffi::rust_call({{ iface.ffi_init_callback().name() }}, nullptr, callback_stub);
        });

        return callbacks.insert(impl);
    }

    int uniffi::{{ class_name }}::callback_stub(uint64_t handle, uint32_t method, uint8_t *args_data, int32_t args_len, RustBuffer *buf_ptr) {
        ForeignBytes bytes = { args_len, args_data };
        auto buf = uniffi::rustbuffer_from_bytes(bytes);
        auto stream = uniffi::RustStream(&buf);

        switch (method) {
        case 0:
            callbacks.erase(handle);
            break;
        {% for method in iface.methods() -%}
        case {{ loop.index }}:
            {% if method.throws_type().is_some() %}{{ "try {" }}{% endif %}
            auto impl = lift(handle);
            {% if method.return_type().is_some() %}auto ret = {% endif -%}
            impl->{{ method.name() }}(
                {%- for arg in method.arguments() %}
                {{- arg|read_fn }}{%- if !loop.last %}, {% else %}{% endif %}
                {% endfor -%}
            );
            {%- match method.return_type() %}
            {% when Some with (return_type) %}

            *buf_ptr = uniffi::rustbuffer_alloc({{ return_type|allocation_size_fn }}(ret));

            auto out_stream = uniffi::RustStream(buf_ptr);

            {{ return_type|write_fn }}(out_stream, ret);
            {% else %}
            {% endmatch %}
            {%- match method.throws_type() %}
            {% when Some with (e) -%}
            } catch (const {{ e|type_name }} &ex) {
                *buf_ptr = uniffi::{{ e|ffi_converter_name }}::lower(ex);
                return 1;
            }
            {%- else %}
            {%- endmatch %}
            break;
        {%- endfor %}
        }

        return 0;
    }
}
