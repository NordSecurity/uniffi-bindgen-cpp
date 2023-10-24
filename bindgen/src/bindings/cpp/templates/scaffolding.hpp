#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct ForeignBytes {
    int32_t len;
    uint8_t *data;
};

struct RustBuffer {
    int32_t capacity;
    int32_t len;
    uint8_t *data;
};

struct RustCallStatus {
    int8_t code;
    RustBuffer error_buf;
};

typedef int ForeignCallback(uint64_t handle, uint32_t method, uint8_t *args_data, int32_t args_len, RustBuffer *buf_ptr);

{% for func in ci.iter_ffi_function_definitions() %}
{%- match func.return_type() -%}
{% when Some with (return_type) %}{{ return_type|ffi_type_name }} {% when None %}void {% endmatch %}{{ func.name() }}(
{%- for arg in func.arguments() %}{{- arg.type_().borrow()|ffi_type_name }} {{ arg.name() }}{% if !loop.last %}, {% endif %}{% endfor %}
{%- if func.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif -%}
);
{% endfor %}

#ifdef __cplusplus
}
#endif
