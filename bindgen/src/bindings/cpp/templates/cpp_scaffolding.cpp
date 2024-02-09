{%- import "macros.cpp" as macros %}
{% let namespace = ci.namespace() %}
{% match config.namespace %}
{% when Some with (ns) %}
{% let namespace = ns %}
{% else %}
{% endmatch %}

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

using namespace {{ namespace }};

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

struct RustStreamBuffer: std::basic_streambuf<uint8_t> {
    RustStreamBuffer(RustBuffer *buf) {
        this->setg(buf->data, buf->data, buf->data + buf->len);
        this->setp(buf->data, buf->data + buf->capacity);
    }
    ~RustStreamBuffer() = default;

private:
    RustStreamBuffer() = delete;
    RustStreamBuffer(const RustStreamBuffer &) = delete;
    RustStreamBuffer(RustStreamBuffer &&) = delete;

    RustStreamBuffer &operator=(const RustStreamBuffer &) = delete;
    RustStreamBuffer &operator=(RustStreamBuffer &&) = delete;
};

struct RustStream: std::basic_iostream<uint8_t> {
    RustStream(RustBuffer *buf):
        streambuf(RustStreamBuffer(buf)), std::basic_iostream<uint8_t>(&streambuf) { }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    RustStream &operator>>(T &val) {
        read(reinterpret_cast<uint8_t *>(&val), sizeof(T));

        if (std::endian::native != std::endian::big) {
            auto bytes = reinterpret_cast<char *>(&val);

            std::reverse(bytes, bytes + sizeof(T));
        }

        return *this;
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    RustStream &operator<<(T val) {
        if (std::endian::native != std::endian::big) {
            auto bytes = reinterpret_cast<char *>(&val);

            std::reverse(bytes, bytes + sizeof(T));
        }

        write(reinterpret_cast<uint8_t *>(&val), sizeof(T));

        return *this;
    }
private:
    RustStreamBuffer streambuf;
};

{%- include "handle_map.cpp" %}

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
{%- else %}
{%- if e.is_flat() %}
{% include "enum_conv.hpp" %}
{% endif %}
{%- endif %}
{%- when Type::Object { module_path, name, imp } %}
HandleMap<{{ typ|canonical_name }}> {{ name }}_map;
{%- when Type::CallbackInterface { module_path, name } %}
{% include "scaffolding/callback.hpp" %}
{% else %}
{%- endmatch %}
{%- endfor %}

#ifdef __cplusplus
extern "C" {
#endif

RustBuffer {{ ci.ffi_rustbuffer_alloc().name() }}(int32_t size, RustCallStatus *out_status) {
    out_status->code = 0;

    RustBuffer buf = {
        .capacity = size,
        .len = size,
        .data = new uint8_t[size],
    };

    return std::move(buf);
}

RustBuffer {{ ci.ffi_rustbuffer_from_bytes().name() }}(ForeignBytes bytes, RustCallStatus *out_status) {
    out_status->code = 0;

    RustBuffer buf = {
        .capacity = bytes.len,
        .len = bytes.len,
        .data = new uint8_t[bytes.len],
    };

    std::copy(bytes.data, bytes.data + bytes.len, buf.data);

    return std::move(buf);
}

void {{ ci.ffi_rustbuffer_free().name() }}(RustBuffer buf, RustCallStatus *out_status) {
    out_status->code = 0;

    delete[] buf.data;
}

RustBuffer {{ ci.ffi_rustbuffer_reserve().name() }}(RustBuffer buffer, int32_t additional, RustCallStatus *out_status) {
    out_status->code = 0;

    RustBuffer buf = {
        .capacity = buffer.capacity + additional,
        .len = buffer.len,
        .data = (uint8_t*)std::realloc(buffer.data, buffer.capacity + additional),
    };

    return std::move(buf);
}

{% for func in ci.function_definitions() %}
{% let ffi_func = func.ffi_func() %}
{%- match ffi_func.return_type() -%}
{% when Some with (return_type) %}{{ return_type|ffi_type_name }} {% when None %}void {% endmatch %}{{ ffi_func.name() }}(
{%- for arg in ffi_func.arguments() %}
{{- arg.type_().borrow()|ffi_type_name }} {{ arg.name() }}{% if !loop.last || ffi_func.has_rust_call_status_arg() %}, {% endif -%}
{% endfor %}
{%- if ffi_func.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif -%}
) {
    {%- if ffi_func.has_rust_call_status_arg() %}
    out_status->code = 0;
    {% endif -%}

    {% match func.return_type() %}
    {% when Some with (return_type) %}
    auto ret = {{ namespace }}::{{ func.name() }}(
    {%- for arg in func.arguments() %}
    {{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
    {% endfor %});
    return {{ return_type|lower_fn }}(ret);
    {% when None %}
    {{ namespace }}::{{ func.name() }}(
    {%- for arg in func.arguments() %}
    {{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
    {% endfor %});
    {% endmatch %}
}
{% endfor %}

{% for func in ci.callback_interface_definitions() %}
{% let ffi_func = func.ffi_init_callback() %}
void {{ ffi_func.name() }}(ForeignCallback callback_stub, RustCallStatus *out_status) {
    out_status->code = 0;

    {{ func|ffi_converter_name }}::fn_handle.store(reinterpret_cast<uint64_t>(callback_stub));
}
{% endfor %}

{% for obj in ci.object_definitions() %}

{% for ctor in obj.constructors() %}
{% let ffi_ctor = ctor.ffi_func() %}
{%- match ffi_ctor.return_type() -%}
{% when Some with (return_type) %}{{ return_type|ffi_type_name }} {% when None %}void {% endmatch %}{{ ffi_ctor.name() }}(
{%- for arg in ffi_ctor.arguments() %}
{{- arg.type_().borrow()|ffi_type_name }} {{ arg.name() }}{% if !loop.last || ffi_ctor.has_rust_call_status_arg() %}, {% endif -%}
{% endfor %}
{%- if ffi_ctor.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif -%}
) {
    {%- if ffi_ctor.has_rust_call_status_arg() %}
    out_status->code = 0;
    {% endif -%}

    std::shared_ptr<{{ obj.name() }}> obj = std::make_shared<{{ obj.name() }}>(
    {%- for arg in ctor.arguments() %}
    {{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
    {% endfor %});

    return (void*){{ obj.name() }}_map.insert(obj);
}
{% endfor %}

{% let ffi_dtor = obj.ffi_object_free() %}
{%- match ffi_dtor.return_type() -%}
{% when Some with (return_type) %}{{ return_type|ffi_type_name }} {% when None %}void {% endmatch %}{{ ffi_dtor.name() }}(
{%- for arg in ffi_dtor.arguments() %}
{{- arg.type_().borrow()|ffi_type_name }} {{ arg.name() }}{% if !loop.last || ffi_dtor.has_rust_call_status_arg() %}, {% endif -%}
{% endfor %}
{%- if ffi_dtor.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif -%}
) {
    {%- if ffi_dtor.has_rust_call_status_arg() %}
    out_status->code = 0;
    {% endif -%}

    {{ obj.name() }}_map.erase((uint64_t)ptr);
}

{% for func in obj.methods() %}
{% let ffi_func = func.ffi_func() %}
{%- match ffi_func.return_type() -%}
{% when Some with (return_type) %}{{ return_type|ffi_type_name }} {% when None %}void {% endmatch %}{{ ffi_func.name() }}(
{%- for arg in ffi_func.arguments() %}
{{- arg.type_().borrow()|ffi_type_name }} {{ arg.name() }}{% if !loop.last || ffi_func.has_rust_call_status_arg() %}, {% endif -%}
{% endfor %}
{%- if ffi_func.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif -%}
) {
    {%- if ffi_func.has_rust_call_status_arg() %}
    out_status->code = 0;
    {% endif -%}

    auto obj = {{ obj.name() }}_map.at((uint64_t)ptr);

    {% match func.return_type() %}
    {% when Some with (return_type) %}
    auto ret = obj->{{ func.name() }}(
    {%- for arg in func.arguments() %}
    {{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
    {% endfor %});
    return {{ return_type|lower_fn }}(ret);
    {% when None %}
    obj->{{ func.name() }}(
    {%- for arg in func.arguments() %}
    {{- arg|lift_fn }}({{ arg.name()|var_name }}){% if !loop.last %}, {% endif -%}
    {% endfor %});
    {% endmatch %}
}
{% endfor %}
{% endfor %}

{% for func in ci.iter_futures_ffi_function_definitons() %}
{%- match func.return_type() -%}
{% when Some with (return_type) %}{{ return_type|ffi_type_name }} {% when None %}void {% endmatch %}{{ func.name() }}(
{%- for arg in func.arguments() %}
{{- arg.type_().borrow()|ffi_type_name }} {{ arg.name() }}{% if !loop.last || func.has_rust_call_status_arg() %}, {% endif -%}
{% endfor %}
{%- if func.has_rust_call_status_arg() %}RustCallStatus *out_status{% endif -%}
) {
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
uint16_t {{ checksum.0 }}() { return {{ checksum.1 }}; }
{% endfor %}

{% let contract_fn = ci.ffi_uniffi_contract_version() %}
uint32_t {{ contract_fn.name() }}() { return {{ ci.uniffi_contract_version() }}; }

#ifdef __cplusplus
}
#endif

RustBuffer rustbuffer_alloc(int32_t size) {
    RustCallStatus status = { 0 };

    return {{ ci.ffi_rustbuffer_alloc().name() }}(
        size,
        &status
    );
}

RustBuffer rustbuffer_from_bytes(const ForeignBytes& bytes) {
    RustCallStatus status = { 0 };

    return {{ ci.ffi_rustbuffer_from_bytes().name() }}(
        bytes,
        &status
    );
}

void rustbuffer_free(RustBuffer& buf) {
    RustCallStatus status = { 0 };

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
{%- else %}
{%- if e.is_flat() %}
{% include "enum_tmpl.cpp" %}
{%- endif %}
{%- endif %}
{%- when Type::Object { module_path, name, imp } %}
{%- when Type::CallbackInterface { module_path, name } %}
{% include "scaffolding/callback.cpp" %}
{%- else %}
{%- endmatch %}
{%- endfor %}
