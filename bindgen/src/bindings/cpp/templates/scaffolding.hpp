#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNIFFI_CPP_INTERNALSTRUCTS
#define UNIFFI_CPP_INTERNALSTRUCTS
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

#endif

typedef int ForeignCallback(uint64_t handle, uint32_t method, uint8_t *args_data, int32_t args_len, RustBuffer *buf_ptr);

{% for func in ci.iter_ffi_function_definitions() %}
{%- match func.return_type() -%}
{% when Some with (return_type) %}{{ return_type|ffi_type_name }} {% when None %}void {% endmatch %}{{ func.name() }}(
{%- for arg in func.arguments() %}
{{- arg.type_().borrow()|ffi_type_name }} {{ arg.name() }}{% if !loop.last || func.has_rust_call_status_arg() %}, {% endif -%}
{% endfor %}
{%- if func.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif -%}
);
{% endfor %}
#ifdef __cplusplus
}
#endif
