{%- let namespace = ci.namespace() %}
{%- let obj = ci|get_object_definition(name) %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let canonical_type_name = typ|canonical_name %}

namespace {{ namespace }} {
    {{ canonical_type_name }}::{{ canonical_type_name }}(void *ptr): instance(ptr) {}

    {% match obj.primary_constructor() %}
    {%- when Some with (ctor) %}
    {{ type_name }} {{ canonical_type_name }}::init({% call macros::param_list(ctor) %}) {
        return {{ type_name }}(new {{ canonical_type_name }}({% call macros::rust_call(ctor) %}));
    }
    {%- else %}
    {%- endmatch %}
    {%- for ctor in obj.alternate_constructors() %}
    {{ type_name }} {{ canonical_type_name }}::{{ ctor.name() }}({% call macros::param_list(ctor) %}) {
        return {{ type_name }}(new {{ canonical_type_name }}({% call macros::rust_call(ctor) %}));
    }
    {%- endfor %}

    {% for method in obj.methods() %}
    {%- match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name }} {% else %}void {% endmatch -%}
    {{ canonical_type_name }}::{{ method.name()|fn_name }}({% call macros::param_list(method) %}) {
        {% match method.return_type() %}
        {%- when Some with (return_type) %}
        return {{ namespace }}::uniffi::{{ return_type|lift_fn }}({% call macros::rust_call_with_prefix("this->instance", method) %});
        {% else %}
        {% call macros::rust_call_with_prefix("this->instance", method) %};
        {%- endmatch %}
    }
    {% endfor %}

    {{ canonical_type_name }}::~{{ canonical_type_name }}() {
        {{ namespace }}::uniffi::rust_call(
            {{ obj.ffi_object_free().name() }},
            nullptr,
            this->instance
        );
    }

    {{ type_name }} uniffi::{{ ffi_converter_name }}::lift(void *ptr) {
        return {{ type_name }}(new {{ canonical_type_name }}(ptr));
    }

    void *uniffi::{{ ffi_converter_name }}::lower(const {{ type_name }} &obj) {
        return obj->instance;
    }

    {{ type_name }} uniffi::{{ ffi_converter_name }}::read(RustStream &stream) {
        std::uintptr_t ptr;

        stream >> ptr;

        return {{ type_name }}(new {{ canonical_type_name }}(reinterpret_cast<void *>(ptr)));
    }

    void uniffi::{{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &obj) {
        stream << reinterpret_cast<std::uintptr_t>(obj->instance);
    }

    int32_t uniffi::{{ ffi_converter_name }}::allocation_size(const {{ type_name }} &) {
        return 8;
    }
}
