{% macro fn_prologue(ci, func, ffi_func) -%}
{%- if ffi_func.has_rust_call_status_arg() %}
    out_status->code = UNIFFI_CALL_STATUS_OK;
{%- endif %}
    try {
{% endmacro %}

{% macro fn_epilogue(ci, func, ffi_func) -%}
{%- if ffi_func.has_rust_call_status_arg() %}
{%- if func.throws() %}
{%- let err_type = func.throws_type().unwrap()|type_name %}
{%- let err_enum = ci.get_enum_definition(err_type).unwrap() %}
{%- for variant in err_enum.variants() %}
{%- let converter_name = err_enum|ffi_converter_name %}
    } catch (const {{ err_type|to_lower_snake_case }}::{{ variant.name() }} &e) {
        out_status->code = UNIFFI_CALL_STATUS_ERROR;
        out_status->error_buf = {{ converter_name }}{{ variant.name() }}::lower(e);
{%- endfor %}
{%- endif %}
    } catch (const std::exception &e) {
        out_status->code = UNIFFI_CALL_STATUS_PANIC;
        out_status->error_buf = {{ Type::String.borrow()|ffi_converter_name }}::lower(e.what());
    } catch (...) {
        out_status->code = UNIFFI_CALL_STATUS_PANIC;
    }
{% match func.return_type() %}
{% when Some with (return_type) %}
    return {};
{% else %}
{% endmatch %}
{%- endif %}
{% endmacro %}

{% macro invoke_native_fn(scaffolding_fn, namespace) %}
{% match func.return_type() %}
{% when Some with (return_type) %}
auto ret = {{ namespace }}::{{ scaffolding_fn.name() }}(
{%- for arg in scaffolding_fn.arguments() %}
{{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
{% endfor %});
return {{ return_type|lower_fn }}(ret);
{% when None %}
{{ namespace }}::{{ scaffolding_fn.name() }}(
{%- for arg in scaffolding_fn.arguments() %}
{{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
{% endfor %});
{% endmatch %}
{% endmacro %}

{% macro invoke_native_fn_obj(scaffolding_fn) %}
{% match func.return_type() %}
{% when Some with (return_type) %}
auto ret = obj->{{ scaffolding_fn.name() }}(
{% if scaffolding_fn.takes_self_by_arc() %}obj{% if !scaffolding_fn.arguments().is_empty() %},{% endif %}{% endif %}
{%- for arg in scaffolding_fn.arguments() %}
{{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
{% endfor %});
return {{ return_type|lower_fn }}(ret);
{% when None %}
obj->{{ scaffolding_fn.name() }}(
{% if scaffolding_fn.takes_self_by_arc() %}obj{% if !scaffolding_fn.arguments().is_empty() %},{% endif %}{% endif %}
{%- for arg in scaffolding_fn.arguments() %}
{{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
{% endfor %});
{% endmatch %}
{% endmacro %}

{% macro fn_definition(ffi_func) %}
{% match ffi_func.return_type() -%}
{% when Some with (return_type) %}{{ return_type|ffi_type_name }} {% when None %}void {% endmatch %}{{ ffi_func.name() }}(
{%- for arg in ffi_func.arguments() %}
{{- arg.type_().borrow()|ffi_type_name }} {{ arg.name() }}{% if !loop.last || ffi_func.has_rust_call_status_arg() %}, {% endif -%}
{% endfor %}
{%- if ffi_func.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif -%})
{% endmacro %}
