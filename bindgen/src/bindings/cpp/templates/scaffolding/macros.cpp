{% macro fn_prologue(ci, func, ffi_func) -%}
{%- if ffi_func.has_rust_call_status_arg() %}
    out_status->code = CALL_STATUS_OK;
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
        out_status->code = CALL_STATUS_ERROR;
        out_status->error_buf = {{ converter_name }}{{ variant.name() }}::lower(e);
{%- endfor %}
{%- endif %}
    } catch (const std::exception &e) {
        out_status->code = CALL_STATUS_PANIC;
        out_status->error_buf = {{ Type::String.borrow()|ffi_converter_name }}::lower(e.what());
    } catch (...) {
        out_status->code = CALL_STATUS_PANIC;
    }
{% match func.return_type() %}
{% when Some with (return_type) %}
    return {};
{% else %}
{% endmatch %}
{%- endif %}
{% endmacro %}
