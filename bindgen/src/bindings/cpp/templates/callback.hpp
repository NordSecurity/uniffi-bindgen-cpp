{%- let class_name = type_name|class_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let trait_impl = canonical_type_name|callback_interface_name %}

{% include "iface.hpp" %}

namespace uniffi {
    struct {{ trait_impl }} {
        static void uniffi_free(uint64_t uniffi_handle);
        static uint64_t uniffi_clone(uint64_t uniffi_handle);
        static void init();

        {%- for (ffi_callback, method) in vtable_methods.iter() %}
        static {% call macros::ffi_return_type(ffi_callback) %} {{ method.name()|var_name }}({% call macros::arg_list_ffi_decl_xx(ffi_callback) %});
        {%- endfor %}

    private:
        static inline {{ vtable|ffi_type_name }} vtable = {{ vtable|ffi_type_name }} {
            .uniffi_free = reinterpret_cast<void *>(&uniffi_free),
            .uniffi_clone = reinterpret_cast<void *>(&uniffi_clone),
            {%- for (ffi_callback, meth) in vtable_methods.iter() %}
            .{{ meth.name()|var_name }} = reinterpret_cast<void *>(&{{ meth.name()|var_name }}),
            {%- endfor %}
        };
    };
}
