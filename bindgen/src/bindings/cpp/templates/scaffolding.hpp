#pragma once

{%- import "macros.cpp" as macros %}

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
    uint64_t capacity;
    uint64_t len;
    uint8_t *data;
};

struct RustCallStatus {
    int8_t code;
    RustBuffer error_buf;
};

#endif

{%- for def in ci.ffi_definitions() %}
{%- match def %}
{%- when FfiDefinition::CallbackFunction(callback) %}
{% call macros::ffi_return_type(callback) %} {{ callback.name()}}(
    {% call macros::arg_list_ffi_decl_xx(callback) %}
);
{%- when FfiDefinition::Struct(ffi_struct) %}
struct {{ ffi_struct.name()|ffi_struct_name }} {
    {%- for field in ffi_struct.fields() %}
    {{ field.type_().borrow()|ffi_type_name }} {{ field.name()|var_name }};
    {%- endfor %}
};
{%- when FfiDefinition::Function(func) %}
{%- match func.return_type() -%}
{% when Some with (return_type) %}{{ return_type|ffi_type_name }} {% when None %}void {% endmatch %}{{ func.name() }}(
{%- for arg in func.arguments() %}
{{- arg.type_().borrow()|ffi_type_name }} {{ arg.name() }}{% if !loop.last || func.has_rust_call_status_arg() %}, {% endif -%}
{% endfor %}
{%- if func.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif -%}
);
{%- endmatch %}
{%- endfor %}
#ifdef __cplusplus
}
#endif
