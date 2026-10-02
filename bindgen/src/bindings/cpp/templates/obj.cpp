{%- let obj = ci.get_object_definition(name).unwrap() %}
{%- let (interface_name, impl_class_name) = obj|object_names %}
{%- let class_name = type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let canonical_type_name = typ|canonical_name %}

{%- if obj.has_callback_interface() %}
{%- let vtable = obj.vtable().expect("trait interface should have a vtable") %}
{%- let vtable_methods = obj.vtable_methods() %}
{%- let ffi_init_callback = obj.ffi_init_callback() %}
namespace uniffi {
{% include "callback_iface_tmpl.cpp" %}
} // namespace uniffi
{%- endif %}


{{ impl_class_name }}::{{ impl_class_name }}(uint64_t ptr): instance(ptr) {}

{{ impl_class_name }}::{{ impl_class_name }}(const {{ impl_class_name }} &other) : instance(0) {
    if (other.instance) {
        instance = other._uniffi_internal_clone_handle();
    }
}

{% if ci.is_name_used_as_error(name) %}
    void {{ impl_class_name }}::throw_underlying() {
        throw *this;
    }
{% endif %}

{% match obj.primary_constructor() -%}
{%- when Some with (ctor) %}
{{ type_name }} {{ impl_class_name }}::init({% call macros::param_list(ctor) %}) {
    return {{ type_name }}(
        new {{ impl_class_name }}({%- call macros::rust_call(ctor) -%})
    );
}
{% else -%}
{% endmatch -%}

{% for ctor in obj.alternate_constructors() %}
{{ type_name }} {{ impl_class_name }}::{{ ctor.name() }}({% call macros::param_list(ctor) %}) {
    return {{ type_name }}(new {{ impl_class_name }}({% call macros::rust_call(ctor) %}));
}
{% endfor %}

{% call macros::method_defs(obj.methods(), impl_class_name, "this->_uniffi_internal_clone_handle()", "") %}

{{ impl_class_name }}::~{{ impl_class_name }}() {
    uniffi::rust_call(
        {{ obj.ffi_object_free().name() }},
        nullptr,
        this->instance
    );
}

uint64_t {{ impl_class_name }}::_uniffi_internal_clone_handle() const {
    return uniffi::rust_call(
        {{ obj.ffi_object_clone().name() }},
        nullptr,
        this->instance
    );
}

{% call macros::uniffi_trait_defs(obj.uniffi_trait_methods(), impl_class_name, type_name, "this->_uniffi_internal_clone_handle()") %}
