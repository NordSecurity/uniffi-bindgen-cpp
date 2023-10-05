{%- macro rust_call(func) -%}
    {{ namespace }}::uniffi::rust_call(
        {{ func.ffi_func().name() }},
{%- match func.throws_type() -%}
{% when Some with (e) %}
        {{ namespace }}::uniffi::{{ e|ffi_converter_name }}::lift
{%- else %}
        nullptr
{%- endmatch -%}
{%- if !func.arguments().is_empty() %},{% else %}{% endif %}
        {% call arg_list_lowered(func) %}
    )
{%- endmacro -%}

{%- macro rust_call_with_prefix(prefix, func) -%}
    {{ namespace }}::uniffi::rust_call(
        {{ func.ffi_func().name() }},
{%- match func.throws_type() -%}
{% when Some with (e) %}
        {{ namespace }}::uniffi::{{ e|ffi_converter_name }}::lift,
{%- else %}
        nullptr,
{%- endmatch %}
        {{ prefix }}
{%- if !func.arguments().is_empty() %},{% else %}{% endif %}
        {% call arg_list_lowered(func) %}
    )
{%- endmacro -%}

{%- macro arg_list_decl(func) %}
{%- for arg in func.arguments() -%}
{{ arg|type_name }} {{ arg.name()|var_name }}
{%- match arg.default_value() %}
{%- when Some with(literal) %} = {{ literal|literal_cpp(arg.as_type().borrow()) }}
{%- else %}
{%- endmatch %}
{%- if !loop.last -%}, {% endif %}
{%- endfor %}
{%- endmacro %}

{%- macro arg_list_lowered(func) %}
    {%- for arg in func.arguments() -%}
        {{ namespace }}::uniffi::{{ arg|lower_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
    {%- endfor %}
{%- endmacro -%}
