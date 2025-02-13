{%- let class_name = type_name|class_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let trait_impl=format!("UniffiCallbackInterface{}", canonical_type_name) %}

{% call macros::docstring_value(interface_docstring, 0) %}
struct {{ interface_name }} {
    virtual ~{{ interface_name }}() {}

    {%- for method in methods.iter() %}
    {%- call macros::docstring(method, 4) %}
    virtual
    {% match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name(ci) }} {% else %}void {% endmatch %}
    {{- method.name()|fn_name }}({% call macros::param_list(method) %}) = 0;
    {%- endfor %}
};

namespace uniffi {
    struct {{ trait_impl }} {
        {%- for (ffi_callback, method) in vtable_methods.iter() %}
        static {% call macros::ffi_return_type(ffi_callback) %} {{ method.name()|var_name }}({% call macros::arg_list_ffi_decl_xx(ffi_callback) %});
        {%- endfor %}

        static void uniffi_free(uint64_t uniffi_handle);
        static void init();
    private:
        static inline {{ vtable|ffi_type_name }} vtable = {{ vtable|ffi_type_name}} {
            {%- for (ffi_callback, meth) in vtable_methods.iter() %}
            .{{ meth.name()|var_name }} = (void*)&{{ meth.name()|var_name }},
            {%- endfor %}
            .uniffi_free = (void*)&uniffi_free
        };
    };
}
