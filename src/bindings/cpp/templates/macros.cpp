{%- macro rust_call(func) -%}
    rust_call(
        {{ func.ffi_func().name() }},
{%- match func.throws_type() -%}
{% when Some with (e) %}
        {{ ci.namespace() }}::uniffi::{{ e|ffi_converter_name }}::lift,
{%- else %}
        nullptr,
{%- endmatch %}
        {% call arg_list_lowered(func) %}
    );
{%- endmacro -%}

{%- macro to_ffi_call_with_prefix(prefix, func) -%}
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
        {{ ci.namespace() }}::uniffi::{{ arg|lower_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
    {%- endfor %}
{%- endmacro -%}
