{%- let namespace = ci.namespace() %}
{%- let obj = ci|get_object_definition(name) %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}

namespace {{ namespace }} {
    {{ class_name }}::{{ class_name }}(void *ptr): instance(ptr) {}

    {%- match obj.primary_constructor() %}
    {%- when Some with (ctor) %}
    {{ class_name }} {{ class_name }}::init ({% call macros::arg_list_decl(ctor) %}) {
        return {{ class_name }}({% call macros::rust_call(ctor) %});
    }
    {%- else %}
    {%- endmatch %}
    {%- for ctor in obj.alternate_constructors() %}
    {{ class_name }} {{ class_name }}::init({% call macros::arg_list_decl(ctor) %}) {
        return {{ class_name }}({% call macros::rust_call(ctor) %});
    }
    {%- endfor %}

    {% for method in obj.methods() %}
    {%- match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name }} {% else %}void {% endmatch -%}
    {{ class_name }}::{{ method.name()|fn_name }}({% call macros::arg_list_decl(method) %}) {
        {% match method.return_type() %}
        {%- when Some with (return_type) %}
        return {{ namespace }}::uniffi::{{ return_type|lift_fn }}({% call macros::rust_call_with_prefix("this->instance", method) %});
        {% else %}
        {% call macros::rust_call_with_prefix("this->instance", method) %};
        {%- endmatch %}
    }
    {% endfor %}

    {{ class_name }} uniffi::{{ ffi_converter_name }}::lift(void *ptr) {
        return {{ class_name }}(ptr);
    }

    void *uniffi::{{ ffi_converter_name }}::lower(const {{ class_name }} &obj) {
        return obj.instance;
    }

    {{ class_name }} uniffi::{{ ffi_converter_name }}::read(RustStream &stream) {
        std::uintptr_t ptr;

        stream >> ptr;

        return {{ class_name }}(reinterpret_cast<void *>(ptr));
    }

    void uniffi::{{ ffi_converter_name }}::write(RustStream &stream, const {{ class_name }} &obj) {
        stream << reinterpret_cast<std::uintptr_t>(obj.instance);
    }

    int32_t uniffi::{{ ffi_converter_name }}::allocation_size(const {{ class_name }} &) {
        return 8;
    }
}
