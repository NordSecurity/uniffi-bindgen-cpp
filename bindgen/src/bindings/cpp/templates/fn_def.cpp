{%- match func.return_type() %}
{%- when Some with (return_type) %}
{{ return_type|type_name(ci) }} {{ func.name()|fn_name }}({% call macros::param_list(func) %}) {
    auto ret = {% call macros::rust_call(func) %};

    return uniffi::{{ return_type|lift_fn }}(ret);
}
{%- when None -%}
void {{ func.name()|fn_name }}({% call macros::param_list(func) %}) {
    {% call macros::rust_call(func) %};
}
{%- endmatch -%}
