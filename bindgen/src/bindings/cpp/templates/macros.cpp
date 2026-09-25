{% macro rust_call(func) -%}
    uniffi::rust_call(
        {{ func.ffi_func().name() }},
{%- match func.throws_type() %}
{% when Some with (e) %}
        uniffi::{{ e|ffi_error_converter_name }}::lift
{%- else %}
        nullptr
{%- endmatch %}
{%- if !func.arguments().is_empty() %}, {% else %}{% endif %}
        {%- call arg_list_lowered(func, true) -%})
{%- endmacro %}

{% macro rust_call_with_prefix(prefix, func, deref) -%}
    uniffi::rust_call(
        {{ func.ffi_func().name() }},
{%- match func.throws_type() %}
{% when Some with (e) %}
        uniffi::{{ e|ffi_error_converter_name }}::lift,
{%- else %}
        nullptr,
{%- endmatch %}
        {{ prefix }}
{%- if !func.arguments().is_empty() %}, {% else %}{% endif %}
        {%- call arg_list_lowered(func, deref) -%})
{%- endmacro %}

{% macro param_list(func) %}
{%- for arg in func.arguments() -%}
{{ arg|parameter(ci) }}
{%- if !loop.last -%}, {% endif -%}
{% endfor -%}
{% endmacro %}


{#- An enum whose variants have no fields is generated as a C++ `enum class`, which cannot
    have member functions, so its methods are emitted as free functions taking the value. #}
{%- macro method_decls(methods, qualifier, suffix) %}
{%- for method in methods %}
{%- call docstring(method, 4) %}
    {{ qualifier }}{% match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name(ci) }} {% else %}void {% endmatch %}
    {{- method.name()|fn_name }}({% call param_list(method) %}){{ suffix }};
{%- endfor %}
{%- endmacro %}

{%- macro method_defs(methods, define_on, self_expr, suffix) %}
{%- for method in methods %}
{% match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name(ci) }} {% else %}void {% endmatch %}
{{- define_on }}::{{ method.name()|fn_name }}({% call param_list(method) %}){{ suffix }} {
    {%- match method.return_type() %}
    {% when Some with (return_type) %}
    return uniffi::{{ return_type|lift_fn }}({% call rust_call_with_prefix(self_expr, method, true) %});
    {%- else %}
    {% call rust_call_with_prefix(self_expr, method, true) -%};
    {%- endmatch %}
}
{%- endfor %}
{%- endmacro %}

{%- macro method_free_decls(methods, param_type) %}
{%- for method in methods %}
{%- call docstring(method, 0) %}
{% match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name(ci) }} {% else %}void {% endmatch %}
{{- method.name()|fn_name }}({{ param_type }} value{% if !method.arguments().is_empty() %}, {% endif %}{% call param_list(method) %});
{%- endfor %}
{%- endmacro %}

{%- macro method_free_defs(methods, param_type, self_expr) %}
{%- for method in methods %}
{% match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name(ci) }} {% else %}void {% endmatch %}
{{- method.name()|fn_name }}({{ param_type }} value{% if !method.arguments().is_empty() %}, {% endif %}{% call param_list(method) %}) {
    {%- match method.return_type() %}
    {% when Some with (return_type) %}
    return uniffi::{{ return_type|lift_fn }}({% call rust_call_with_prefix(self_expr, method, true) %});
    {%- else %}
    {% call rust_call_with_prefix(self_expr, method, true) -%};
    {%- endmatch %}
}
{%- endfor %}
{%- endmacro %}


{#- The uniffi traits that a type exports. These are rendered apart from plain methods
    because their C++ names are not taken from Rust, so they cannot be emitted from a `Method` alone.
    They split into member and free variants for the same reason as the methods above. #}
{%- macro uniffi_trait_decls(uniffi_traits, param_type) %}
{%- for t in uniffi_traits|uniffi_trait_list %}
    /**
     * {{ t.doc }}
     */
    {{ t.return_type|type_name(ci) }} {{ t.name }}({% if t.takes_other %}const {{ param_type }} &other{% endif %}) const;
{%- endfor %}
{%- endmacro %}

{%- macro uniffi_trait_defs(uniffi_traits, define_on, param_type, self_expr) %}
{%- for t in uniffi_traits|uniffi_trait_list %}
{{ t.return_type|type_name(ci) }} {{ define_on }}::{{ t.name }}({% if t.takes_other %}const {{ param_type }} &other{% endif %}) const {
    return uniffi::{{ t.return_type|lift_fn }}({% call rust_call_with_prefix(self_expr, t.method, false) %});
}
{%- endfor %}
{%- endmacro %}

{%- macro uniffi_trait_free_decls(uniffi_traits, param_type) %}
{%- for t in uniffi_traits|uniffi_trait_list %}
/**
 * {{ t.doc }}
 */
{{ t.return_type|type_name(ci) }} {{ t.name }}({{ param_type }} value{% if t.takes_other %}, {{ param_type }} other{% endif %});
{%- endfor %}
{%- endmacro %}

{%- macro uniffi_trait_free_defs(uniffi_traits, param_type, self_expr) %}
{%- for t in uniffi_traits|uniffi_trait_list %}
{{ t.return_type|type_name(ci) }} {{ t.name }}({{ param_type }} value{% if t.takes_other %}, {{ param_type }} other{% endif %}) {
    return uniffi::{{ t.return_type|lift_fn }}({% call rust_call_with_prefix(self_expr, t.method, false) %});
}
{%- endfor %}
{%- endmacro %}


{% macro field_name(field, field_num) %}
{%- if field.name().is_empty() -%}
v{{- field_num -}}
{%- else -%}
{{ field.name()|var_name }}
{%- endif -%}
{%- endmacro %}

{% macro arg_list_lowered(func, deref) %}
{%- for arg in func.arguments() -%}
uniffi::{{ arg|lower_fn }}({% if deref %}{{ arg.as_type()|cpp_deref(ci) }}{% endif %}{{ arg.name()|var_name }})
{%- if !loop.last -%}, {% endif -%}
{% endfor -%}
{% endmacro %}

{%- macro arg_list_ffi_decl_xx(func) %}
    {%- for arg in func.arguments() %}
        {{- arg.type_().borrow()|ffi_type_name }} {{ arg.name()|var_name -}}{%- if !loop.last || func.has_rust_call_status_arg() -%},{%- endif -%}
    {%- endfor %}
    {%- if func.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif %}
{%- endmacro -%}

{%- macro ffi_return_type(func) %}
    {%- match func.return_type() %}
    {%- when Some(return_type) %}{{ return_type|ffi_type_name }}
    {%- when None %}{{ "void" }}
    {%- endmatch %}
{%- endmacro %}

{%- macro docstring_value(maybe_docstring, indent_spaces) %}
{%- match maybe_docstring %}
{%- when Some(docstring) %}
{{ docstring|docstring(indent_spaces) }}
{%- else %}
{%- endmatch %}
{%- endmacro %}

{%- macro docstring(defn, indent_spaces) %}
{%- call docstring_value(defn.docstring(), indent_spaces) %}
{%- endmacro %}
