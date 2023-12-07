{% macro rust_call(func) -%}
    uniffi::rust_call(
        {{ func.ffi_func().name() }},
{%- match func.throws_type() %}
{% when Some with (e) %}
        uniffi::{{ e|ffi_converter_name }}::lift
{%- else %}
        nullptr
{%- endmatch %}
{%- if !func.arguments().is_empty() %}, {% else %}{% endif %}
        {%- call arg_list_lowered(func) -%})
{%- endmacro %}

{% macro rust_call_with_prefix(prefix, func) -%}
    uniffi::rust_call(
        {{ func.ffi_func().name() }},
{%- match func.throws_type() %}
{% when Some with (e) %}
        uniffi::{{ e|ffi_converter_name }}::lift,
{%- else %}
        nullptr,
{%- endmatch %}
        {{ prefix }}
{%- if !func.arguments().is_empty() %}, {% else %}{% endif %}
        {%- call arg_list_lowered(func) -%})
{%- endmacro %}

{% macro param_list(func) %}
{%- for arg in func.arguments() -%}
{{ arg|parameter }}
{%- if !loop.last -%}, {% endif -%}
{% endfor -%}
{% endmacro %}

{% macro arg_list_lowered(func) %}
{%- for arg in func.arguments() -%}
uniffi::{{ arg|lower_fn }}({{ arg.name()|var_name }})
{%- if !loop.last -%}, {% endif -%}
{% endfor -%}
{% endmacro %}

{%- macro docstring(defn, indent_spaces) %}
{%- match defn.docstring() %}
{%- when Some(docstring) %}
{{ docstring|docstring(indent_spaces) }}
{%- else %}
{%- endmatch %}
{%- endmacro %}
