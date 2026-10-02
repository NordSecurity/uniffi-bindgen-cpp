{%- let obj = ci.get_object_definition(name).unwrap() %}
{%- let (interface_name, impl_class_name) = obj|object_names %}
{%- let class_name = type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let methods = obj.methods() %}
{%- let interface_docstring = obj.docstring() %}
{%- if obj.has_callback_interface() %}
{%- let vtable = obj.vtable().expect("trait interface should have a vtable") %}
{%- let vtable_methods = obj.vtable_methods() %}
{%- let ffi_init_callback = obj.ffi_init_callback() %}
{% include "callback.hpp" %}
{%- else if obj.is_trait_interface() %}
{% include "iface.hpp" %}
{%- endif %}

namespace uniffi {
    struct {{ ffi_converter_name|class_name }};
} // namespace uniffi

{%~ call macros::docstring(obj, 0) %}
struct {{ impl_class_name }}{{ obj|object_bases(ci) }} {
    friend uniffi::{{ ffi_converter_name|class_name }};

    {{ impl_class_name }}() = delete;

    {{ impl_class_name }}({{ impl_class_name }} &&) = delete;

    {{ impl_class_name }} &operator=(const {{ impl_class_name }} &) = delete;
    {{ impl_class_name }} &operator=({{ impl_class_name }} &&) = delete;

    ~{{ impl_class_name }}();

    {%- match obj.primary_constructor() %}
    {%- when Some with (ctor) %}
    {%- call macros::docstring(ctor, 4) %}
    static {{ type_name }} init({% call macros::param_list(ctor) %});
    {%- else %}
    {%- endmatch %}

    {%- for ctor in obj.alternate_constructors() %}
    {%- call macros::docstring(ctor, 4) %}
    static {{ type_name }} {{ ctor.name() }}({% call macros::param_list(ctor) %});
    {%- endfor %}

    {%- call macros::method_decls(obj.methods(), "", "") %}

    {%- call macros::uniffi_trait_decls(obj.uniffi_trait_methods(), type_name) %}

    {% if ci.is_name_used_as_error(name) %}
    void throw_underlying();
    {%- endif -%}
private:
    {{ impl_class_name }}(const {{ impl_class_name }} &);

    {{ impl_class_name }}(uint64_t);

    uint64_t _uniffi_internal_clone_handle() const;

    uint64_t instance = 0;
};
