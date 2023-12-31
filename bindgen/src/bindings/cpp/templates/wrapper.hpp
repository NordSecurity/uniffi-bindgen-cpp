#pragma once

#include <bit>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <streambuf>
#include <type_traits>
#include <variant>

{%- for include in self.includes() %}
#include <{{ include }}>
{%- endfor %}


{%~ let namespace = ci.namespace() %}
#include "{{ namespace }}_scaffolding.hpp"

{%- import "macros.cpp" as macros %}
{% call macros::docstring(ci.namespace_definition(), 0) %}
namespace {{ namespace }} {
{%- for typ in ci.iter_types() %}
{%- let type_name = typ|type_name %}
{%- match typ %}
{%- when Type::Enum { module_path, name } %}
{%- let e = ci|get_enum_definition(name) %}
{%- if ci.is_name_used_as_error(name) %}
struct {{ name }};
{%- else %}
{%- if e.is_flat() %}
enum class {{ name }};
{%- endif %}
{%- endif %}
{%- when Type::Record { module_path, name } %} 
struct {{ name }};
{%- when Type::Object { module_path, name, imp } %}
struct {{ name }};
{%- when Type::CallbackInterface { module_path, name } %}
struct {{ name }};
{%- when Type::Custom { module_path, name, builtin } %}
{%- match config.custom_types.get(name.as_str()) %}
{%- when None %}
typedef {{ builtin|type_name }} {{ name }};
{%- when Some with (type_config) %}
{%- match type_config.type_name %}
{%- when Some with (type_name) %}
typedef {{ type_name }} {{ name }};
{%- else %}
{%- endmatch %}
{%- endmatch %}
{%- else %}
{%- endmatch %}
{%- endfor %}

{%- for typ in self.sorted_types(ci.iter_types()) %}
{%- let type_name = typ|type_name %}
{%- match typ %}
{%- when Type::Enum { module_path, name } %}
{%- let e = ci|get_enum_definition(name) %}
{%- if ci.is_name_used_as_error(name) %}
{% include "err.hpp" %}
{%- else %}
{% include "enum.hpp" %}
{%- endif %}
{%- when Type::Record { module_path, name } %}
{% include "rec.hpp" %}
{%- when Type::CallbackInterface { module_path, name } %}
{% include "callback.hpp" %}
{%- when Type::Object { module_path, name, imp } %}
{% include "obj.hpp" %}
{%- else %}
{%- endmatch %}
{%- endfor %}

namespace uniffi {
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

RustBuffer rustbuffer_alloc(int32_t);
RustBuffer rustbuffer_from_bytes(const ForeignBytes &);
void rustbuffer_free(RustBuffer);

{%- if self.contains_callbacks(ci.iter_types()) %}
template <typename T> struct HandleMap {
    HandleMap() = default;

    std::shared_ptr<T> at(uint64_t handle) {
        std::lock_guard<std::mutex> guard(this->mutex);

        return this->map.at(handle);
    }

    uint64_t insert(std::shared_ptr<T> impl) {
        std::lock_guard<std::mutex> guard(this->mutex);

        auto handle = this->cur_handle;

        this->map.insert({ handle, impl });
        this->cur_handle += 1;

        return handle;
    }

    void erase(uint64_t handle) {
        std::lock_guard<std::mutex> guard(this->mutex);

        this->map.erase(handle);
    }
    private:
        HandleMap(const HandleMap<T> &) = delete;
        HandleMap(HandleMap<T> &&) = delete;

        HandleMap<T> &operator=(const HandleMap<T> &) = delete;
        HandleMap<T> &operator=(HandleMap<T> &&) = delete;

        std::mutex mutex;
        uint64_t cur_handle = 0;
        std::map<uint64_t, std::shared_ptr<T>> map;
};
{%- endif %}

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
{%- when Type::Enum { module_path, name } %}
{%- let e = ci|get_enum_definition(name) %}
{%- if ci.is_name_used_as_error(name) %}
{% include "err_conv.hpp" %}
{%- else %}
{% include "enum_conv.hpp" %}
{%- endif %}
{%- when Type::Object { module_path, name, imp } %}
{% include "obj_conv.hpp" %}
{%- when Type::Record { module_path, name } %}
{% include "rec_conv.hpp" %}
{%- when Type::Optional { inner_type } %}
{% include "opt_conv.hpp" %}
{%- when Type::Sequence { inner_type } %}
{% include "seq_conv.hpp" %}
{%- when Type::Map { key_type, value_type } %}
{% include "map_conv.hpp" %}
{%- when Type::CallbackInterface { module_path, name } %}
{% include "callback_conv.hpp" %}
{%- when Type::Custom { module_path, name, builtin } %}
{% include "custom.hpp" %}
{%- else %}
{%- endmatch %}
{%- endfor %}
} // namespace uniffi

{%~ for func in ci.function_definitions() %}
{%- call macros::docstring(func, 0) %}
{%- match func.return_type() %}
{%- when Some with (return_type) %}
{{ return_type|type_name }} {{ func.name()|fn_name }}({% call macros::param_list(func) %});
{%- when None %}
void {{ func.name()|fn_name }}({% call macros::param_list(func) %});
{%- endmatch %}
{%- endfor %}
} // namespace {{ namespace }}