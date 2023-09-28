namespace {{ ci.namespace() }} {
{%- match func.return_type() -%}
{%- when Some with (return_type) %}
{{ return_type|type_name }} {{ func.name()|fn_name }}({%- call macros::arg_list_decl(func) %}) {
    auto ret = {% call macros::rust_call(func) %}

    return {{ ci.namespace() }}::uniffi::{{ return_type|lift_fn }}(ret);
}
{%- when None %}
void {{ func.name()|fn_name }}({%- call macros::arg_list_decl(func) %}) {
    {% call macros::rust_call(func) %};
}
{%- endmatch %}
}
