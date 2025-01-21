{%- let type_name = typ|type_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let class_name = ffi_converter_name|class_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let trait_impl=format!("UniffiCallbackInterface{}", canonical_type_name) %}

{%- for (ffi_callback, method) in vtable_methods.iter() %}
 {% call macros::ffi_return_type(ffi_callback) %} {{ trait_impl}}::{{ method.name()|var_name }}({% call macros::arg_list_ffi_decl_xx(ffi_callback) %}) {
    auto obj = {{ ffi_converter_name }}::handle_map.at(uniffi_handle);

    auto make_call = [&]() {% match method.return_type() %}{% when Some(t) %}-> {{ t|type_name }}{% when None %}{% endmatch %} {
        {%- for arg in method.arguments() %}
        auto arg{{ loop.index0 }} = {{- arg|lift_fn }}({{ arg.name()|var_name }});
        {%- endfor -%}

        {%- if method.return_type().is_some() %}return {% endif -%}
         obj->{{ method.name()|var_name }}(
        {%- for arg in method.arguments() %}
        std::move(arg{{ loop.index0 }}){%- if !loop.last %}, {% else %}{% endif %}
        {%- endfor -%}
        );
    };

    {% match method.return_type() %}
    {% when Some(t) %}
    auto write_value = [&]({{ t|type_name }} v) {
        uniffi_out_return = {{ t|lower_fn }}(v);
    };
    {% when None %}
    auto write_value = [](){};
    {% endmatch %}

    {% match method.throws_type() %}
    {% when Some(error) %}
        rust_call_trait_interface_with_error<{{ error|type_name }}>(out_status, make_call, write_value, {{ error|lower_fn }});
    {% when None %}
        rust_call_trait_interface(out_status, make_call, write_value);
    {% endmatch %}
}
{%- endfor %}

void {{ trait_impl }}::uniffi_free(uint64_t uniffi_handle) {
    {{ ffi_converter_name }}::handle_map.erase(uniffi_handle);
}

void {{ trait_impl }}::init() {
    {{ ffi_init_callback.name() }}(vtable);
}
