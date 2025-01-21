{%- let obj = ci|get_object_definition(name) %}
{%- let (interface_name, impl_class_name) = obj|object_names %}
{%- let type_name = typ|type_name %}
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


{{ impl_class_name }}::{{ impl_class_name }}(void *ptr): instance(ptr) {}

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

{%- for method in obj.methods() %}
{% match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name }} {% else %}void {% endmatch -%}
{{ impl_class_name }}::{{ method.name()|fn_name }}({% call macros::param_list(method) %}) {
    auto ptr = this->uniffi_clone_pointer();
    {%- match method.return_type() %}
    {% when Some with (return_type) %}
    return uniffi::{{ return_type|lift_fn }}({% call macros::rust_call_with_prefix("ptr", method) %});
    {%- else %}
    {% call macros::rust_call_with_prefix("ptr", method) -%};
    {%- endmatch %}
}
{%- endfor %}

{{ impl_class_name }}::~{{ impl_class_name }}() {
    uniffi::rust_call(
        {{ obj.ffi_object_free().name() }},
        nullptr,
        this->instance
    );
}

void *{{ impl_class_name }}::uniffi_clone_pointer() const {
    return uniffi::rust_call(
        {{ obj.ffi_object_clone().name() }},
        nullptr,
        this->instance
    );
}

{%- for method in obj.uniffi_traits() %}
{% match method %}
{% when UniffiTrait::Display { fmt } %}
std::string {{ impl_class_name }}::to_string() const {
    return uniffi::{{ Type::String.borrow()|lift_fn }}({% call macros::rust_call_with_prefix("this->uniffi_clone_pointer()", fmt) %});
}
{% when UniffiTrait::Debug { fmt } %}
std::string {{ impl_class_name }}::to_debug_string() const {
    return uniffi::{{ Type::String.borrow()|lift_fn }}({% call macros::rust_call_with_prefix("this->uniffi_clone_pointer()", fmt) %});
}
{% when UniffiTrait::Eq { eq, ne } %}
bool {{ impl_class_name }}::eq(const {{ type_name }} &other) const {
    return uniffi::{{ Type::Boolean.borrow()|lift_fn }}({% call macros::rust_call_with_prefix("this->uniffi_clone_pointer()", eq) %});
}
bool {{ impl_class_name }}::ne(const {{ type_name }} &other) const {
    return uniffi::{{ Type::Boolean.borrow()|lift_fn }}({% call macros::rust_call_with_prefix("this->uniffi_clone_pointer()", ne) %});
}
{% when UniffiTrait::Hash { hash } %}
uint64_t {{ impl_class_name }}::hash() const {
    return uniffi::{{ Type::UInt64.borrow()|lift_fn }}({% call macros::rust_call_with_prefix("this->uniffi_clone_pointer()", hash) %});
}
{% endmatch %}
{%- endfor %}
