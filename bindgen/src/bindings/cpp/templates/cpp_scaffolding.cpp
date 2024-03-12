#ifndef UNIFFI_EXPORT
#if defined(_WIN32) || defined(_WIN64)
#define UNIFFI_EXPORT __declspec(dllexport)
#else
#define UNIFFI_EXPORT __attribute__((visibility("default")))
#endif
#endif

#include <stdio.h>
#include <stdint.h>
#include <bit>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <atomic>
#include <map>
#include <mutex>
#include <cstring>

{% let namespace = ci.namespace() %}
{% match config.namespace %}
{% when Some with (ns) %}
{% let namespace = ns %}
{% else %}
{% endmatch %}

namespace uniffi_{{ namespace }} {

{%- import "scaffolding/macros.cpp" as macros %}
template <class> inline constexpr bool always_false_v = false;

using namespace {{ namespace }};

constexpr int8_t UNIFFI_CALL_STATUS_OK = 0;
constexpr int8_t UNIFFI_CALL_STATUS_ERROR = 1;
constexpr int8_t UNIFFI_CALL_STATUS_PANIC = 2;

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

{%- include "rust_buf_stream.cpp" %}
{%- include "scaffolding/object_map.cpp" %}

{%- for typ in ci.iter_types() %}
{%- match typ %}
{%- when Type::Boolean %}
{% include "bool_conv.hpp" %}
{%- when Type::UInt8 %}
{% include "arith_conv.hpp" %}
{%- when Type::Int8 %}
{% include "arith_conv.hpp" %}
{%- when Type::UInt16 %}
{% include "arith_conv.hpp" %}
{%- when Type::Int16 %}
{% include "arith_conv.hpp" %}
{%- when Type::UInt32 %}
{% include "arith_conv.hpp" %}
{%- when Type::Int32 %}
{% include "arith_conv.hpp" %}
{%- when Type::UInt64 %}
{% include "arith_conv.hpp" %}
{%- when Type::Int64 %}
{% include "arith_conv.hpp" %}
{%- when Type::Float32 %}
{% include "arith_conv.hpp" %}
{%- when Type::Float64 %}
{% include "arith_conv.hpp" %}
{%- when Type::Bytes %}
{% include "bytes_conv.hpp" %}
{%- when Type::String %}
{% include "str_conv.hpp" %}
{%- when Type::Timestamp %}
{% include "timestamp_conv.hpp" %}
{%- when Type::Duration %}
{% include "duration_conv.hpp" %}
{%- when Type::Record { module_path, name } %}
{% include "rec_conv.hpp" %}
{%- when Type::Optional { inner_type } %}
{% include "opt_conv.hpp" %}
{%- when Type::Sequence { inner_type } %}
{% include "seq_conv.hpp" %}
{%- when Type::Map { key_type, value_type } %}
{% include "map_conv.hpp" %}
{%- when Type::Enum { module_path, name } %}
{%- let e = ci|get_enum_definition(name) %}
{%- if ci.is_name_used_as_error(name) %}
{% for variant in e.variants() %}
{% include "scaffolding/err.hpp" %}
{% endfor %}
{%- else %}
{% include "enum_conv.hpp" %}
{%- endif %}
{%- when Type::Object { module_path, name, imp } %}
{% include "scaffolding/obj.hpp" %}
ObjectMap<{{ typ|canonical_name }}> {{ name }}_map;
{%- when Type::CallbackInterface { module_path, name } %}
{% include "scaffolding/callback.hpp" %}
{% else %}
{%- endmatch %}
{%- endfor %}

#ifdef __cplusplus
extern "C" {
#endif

UNIFFI_EXPORT RustBuffer {{ ci.ffi_rustbuffer_alloc().name() }}(int32_t size, RustCallStatus *out_status) {
    out_status->code = UNIFFI_CALL_STATUS_OK;

    RustBuffer buf = {
        .capacity = size,
        .len = size,
        .data = new uint8_t[size],
    };

    return std::move(buf);
}

UNIFFI_EXPORT RustBuffer {{ ci.ffi_rustbuffer_from_bytes().name() }}(ForeignBytes bytes, RustCallStatus *out_status) {
    out_status->code = UNIFFI_CALL_STATUS_OK;

    RustBuffer buf = {
        .capacity = bytes.len,
        .len = bytes.len,
        .data = new uint8_t[bytes.len],
    };

    std::copy(bytes.data, bytes.data + bytes.len, buf.data);

    return std::move(buf);
}

UNIFFI_EXPORT void {{ ci.ffi_rustbuffer_free().name() }}(RustBuffer buf, RustCallStatus *out_status) {
    out_status->code = UNIFFI_CALL_STATUS_OK;

    delete[] buf.data;
}

UNIFFI_EXPORT RustBuffer {{ ci.ffi_rustbuffer_reserve().name() }}(RustBuffer buffer, int32_t additional, RustCallStatus *out_status) {
    out_status->code = UNIFFI_CALL_STATUS_OK;

    RustBuffer buf = {
        .capacity = buffer.capacity + additional,
        .len = buffer.len,
        .data = (uint8_t*)std::realloc(buffer.data, buffer.capacity + additional),
    };

    return std::move(buf);
}

{% for func in ci.function_definitions() %}
{% let ffi_func = func.ffi_func() %}
UNIFFI_EXPORT {%- call macros::fn_definition(ffi_func) %} {
    {%- call macros::fn_prologue(ci, func, ffi_func) %}
    {%- call macros::invoke_native_fn(func, namespace) %}
    {%- call macros::fn_epilogue(ci, func, ffi_func) %}
}
{% endfor %}

{% for func in ci.callback_interface_definitions() %}
{% let ffi_func = func.ffi_init_callback() %}
UNIFFI_EXPORT void {{ ffi_func.name() }}(ForeignCallback callback_stub, RustCallStatus *out_status) {
    out_status->code = UNIFFI_CALL_STATUS_OK;

    {{ func|ffi_converter_name }}::fn_handle.store(reinterpret_cast<uint64_t>(callback_stub));
}
{% endfor %}

{% for obj in ci.object_definitions() %}

{% for ctor in obj.constructors() %}
{% let ffi_ctor = ctor.ffi_func() %}
UNIFFI_EXPORT {%- call macros::fn_definition(ffi_ctor) %} {
    {%- if ffi_ctor.has_rust_call_status_arg() %}
    out_status->code = UNIFFI_CALL_STATUS_OK;
    {% endif -%}

    std::shared_ptr<{{ obj.name() }}> obj = std::make_shared<{{ obj.name() }}>(
    {%- for arg in ctor.arguments() %}
    {{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
    {% endfor %});
    return (void*){{ obj.name() }}_map.insert(obj);
}
{% endfor %}

{% let ffi_dtor = obj.ffi_object_free() %}
UNIFFI_EXPORT {%- call macros::fn_definition(ffi_dtor) %} {
    {%- if ffi_dtor.has_rust_call_status_arg() %}
    out_status->code = UNIFFI_CALL_STATUS_OK;
    {% endif -%}

    {{ obj.name() }}_map.erase((uint64_t)ptr);
}

{% for func in obj.methods() %}
{% let ffi_func = func.ffi_func() %}
UNIFFI_EXPORT {%- call macros::fn_definition(ffi_func) %} {
    {%- call macros::fn_prologue(ci, func, ffi_func) %}
    auto obj = {{ obj.name() }}_map.at((uint64_t)ptr);
    {%- call macros::invoke_native_fn_obj(func) %}
    {%- call macros::fn_epilogue(ci, func, ffi_func) %}
}
{% endfor %}
{% endfor %}

{% for func in ci.iter_futures_ffi_function_definitons() %}
UNIFFI_EXPORT {%- call macros::fn_definition(func) %} {
    {%- match func.return_type() %}{% when Some with (return_type) %}
        {% match return_type %}
        {% when FfiType::RustArcPtr(_) %}
        return nullptr;
        {% else %}
        return {{ return_type|ffi_type_name }}{};
        {% endmatch %}
    {% when None %}{% endmatch -%}
}
{% endfor %}

{% for checksum in ci.iter_checksums() %}
UNIFFI_EXPORT uint16_t {{ checksum.0 }}() { return {{ checksum.1 }}; }
{% endfor %}

{% let contract_fn = ci.ffi_uniffi_contract_version() %}
UNIFFI_EXPORT uint32_t {{ contract_fn.name() }}() { return {{ ci.uniffi_contract_version() }}; }

#ifdef __cplusplus
}
#endif

RustBuffer rustbuffer_alloc(int32_t size) {
    RustCallStatus status = { UNIFFI_CALL_STATUS_OK };

    return {{ ci.ffi_rustbuffer_alloc().name() }}(
        size,
        &status
    );
}

RustBuffer rustbuffer_from_bytes(const ForeignBytes& bytes) {
    RustCallStatus status = { UNIFFI_CALL_STATUS_OK };

    return {{ ci.ffi_rustbuffer_from_bytes().name() }}(
        bytes,
        &status
    );
}

void rustbuffer_free(RustBuffer& buf) {
    RustCallStatus status = { UNIFFI_CALL_STATUS_OK };

    {{ ci.ffi_rustbuffer_free().name() }}(
        buf,
        &status
    );
}

{%- for typ in ci.iter_types() %}
{%- let type_name = typ|type_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let contains_object_references = ci.item_contains_object_references(typ) %}

{%- match typ %}
{%- when Type::Boolean %}
{% include "bool_conv.cpp" %}
{%- when Type::UInt8 %}
{% include "arith_conv.cpp" %}
{%- when Type::Int8 %}
{% include "arith_conv.cpp" %}
{%- when Type::UInt16 %}
{% include "arith_conv.cpp" %}
{%- when Type::Int16 %}
{% include "arith_conv.cpp" %}
{%- when Type::UInt32 %}
{% include "arith_conv.cpp" %}
{%- when Type::Int32 %}
{% include "arith_conv.cpp" %}
{%- when Type::UInt64 %}
{% include "arith_conv.cpp" %}
{%- when Type::Int64 %}
{% include "arith_conv.cpp" %}
{%- when Type::Float32 %}
{% include "arith_conv.cpp" %}
{%- when Type::Float64 %}
{% include "arith_conv.cpp" %}
{%- when Type::Bytes %}
{% include "bytes_conv.cpp" %}
{%- when Type::Timestamp %}
{% include "timestamp_helper.cpp" %}
{%- when Type::Duration %}
{% include "duration_helper.cpp" %}
{%- when Type::String %}
{% include "str_conv.cpp" %}
{%- when Type::Record { module_path, name } %}
{% include "rec.cpp" %}
{%- when Type::Optional { inner_type } %}
{% include "opt_tmpl.cpp" %}
{%- when Type::Sequence { inner_type } %}
{% include "seq_tmpl.cpp" %}
{%- when Type::Map { key_type, value_type } %}
{% include "map_tmpl.cpp" %}
{%- when Type::Enum { module_path, name } %}
{%- let e = ci|get_enum_definition(name) %}
{%- if ci.is_name_used_as_error(name) %}
{% for variant in e.variants() %}
{% include "scaffolding/err.cpp" %}
{% endfor %}
{%- else %}
{% include "enum_tmpl.cpp" %}
{%- endif %}
{%- when Type::Object { module_path, name, imp } %}
{% include "scaffolding/obj.cpp" %}
{%- when Type::CallbackInterface { module_path, name } %}
{% include "scaffolding/callback.cpp" %}
{%- else %}
{%- endmatch %}
{%- endfor %}

}
