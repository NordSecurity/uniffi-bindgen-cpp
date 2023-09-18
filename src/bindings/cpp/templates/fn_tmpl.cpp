{%- match func.return_type() -%}
{%- when Some with (return_type) %}

{{ return_type|type_name }} {{ ci.namespace() }}::{{ func.name()|fn_name }}({%- call macros::arg_list_decl(func) %}) {
    RustCallStatus status = { 0 };
    auto ret = {{ func.ffi_func().name() }}({% call macros::arg_list_lowered(func) %}, &status);

    //_uniffi::helpers::_check_rust_call(_uniffi::{{ func.ffi_func().name() }}, {% call macros::arg_list_lowered(func) %});

    {% call macros::check_rust_call(func) %}

    return {{ ci.namespace() }}::uniffi::{{ return_type|lift_fn }}(ret);
}
{%- when None %}

void {{ ci.namespace() }}::{{ func.name()|fn_name }}({%- call macros::arg_list_decl(func) %}) {
    RustCallStatus status = { 0 };

    {% call macros::check_rust_call(func) %};
}
{%- endmatch %}
