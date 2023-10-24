#pragma once

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

{%- let namespace = ci.namespace() %}

#include "{{ namespace }}_scaffolding.hpp"

{%- import "macros.cpp" as macros %}

namespace {{ namespace }} {
    {%- for typ in self.sorted_types(ci.iter_types()) %}
    {%- let type_name = typ|type_name %}
    {%- match typ %}
    {%- when Type::Custom { module_path, name, builtin } %}
    {% include "custom.hpp" %}
    {%- when Type::Record { module_path, name } %}
    {% include "rec.hpp" %}
    {%- when Type::CallbackInterface { module_path, name } %}
    {% include "callback.hpp" %}
    {%- when Type::Enum { module_path, name } %}
    {%- let e = ci|get_enum_definition(name) %}
    {%- if ci.is_name_used_as_error(name) %}
    {%- include "err.hpp" %}
    {%- else %}
    {%- endif %}
    {%- when Type::Object { module_path, name, imp } %}
    {%- include "obj.hpp" %}
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

            template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
            RustStream &operator>>(T &val) {
                read(reinterpret_cast<uint8_t *>(&val), sizeof(T));

                if (std::endian::native != std::endian::big) {
                    auto bytes = reinterpret_cast<char *>(&val);

                    std::reverse(bytes, bytes + sizeof(T));
                }

                return *this;
            }

            template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
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

        void check_rust_call(const RustCallStatus &, std::function<std::exception(RustBuffer)>);

        {% for typ in ci.iter_types() %}
        {% match typ %}
        {% when Type::Boolean %}
        {% include "bool_conv.hpp" %}
        {% when Type::UInt8 %}
        {% include "arith_conv.hpp" %}
        {% when Type::Int8 %}
        {% include "arith_conv.hpp" %}
        {% when Type::UInt16 %}
        {% include "arith_conv.hpp" %}
        {% when Type::Int16 %}
        {% include "arith_conv.hpp" %}
        {% when Type::UInt32 %}
        {% include "arith_conv.hpp" %}
        {% when Type::Int32 %}
        {% include "arith_conv.hpp" %}
        {% when Type::UInt64 %}
        {% include "arith_conv.hpp" %}
        {% when Type::Int64 %}
        {% include "arith_conv.hpp" %}
        {% when Type::Float32 %}
        {% include "arith_conv.hpp" %}
        {% when Type::Float64 %}
        {% include "arith_conv.hpp" %}
        {% when Type::String %}
        {% include "str_conv.hpp" %}
        {%- when Type::Enum { module_path, name } %}
        {%- let e = ci|get_enum_definition(name) %}
        {%- if ci.is_name_used_as_error(name) %}
        {% include "err_conv.hpp" %}
        {%- else %}
        {%- endif %}
        {% when Type::Object { module_path, name, imp } %}
        {% include "obj_conv.hpp" %}
        {% when Type::Record { module_path, name } %}
        {% include "rec_conv.hpp" %}
        {%- when Type::Optional { inner_type } %}
        {% include "opt_conv.hpp" %}
        {%- when Type::Sequence { inner_type } %}
        {% include "seq_conv.hpp" %}
        {%- when Type::CallbackInterface { module_path, name } %}
        {% include "callback_conv.hpp" %}
        {%- else %}
        {%- endmatch %}
        {%- endfor %}
    }

    {% for func in ci.function_definitions() %}
    {%- match func.return_type() %}
    {%- when Some with (return_type) %}
    {{ return_type|type_name }} {{ func.name()|fn_name }}({%- call macros::param_list(func) %});
    {%- when None %}
    void {{ func.name()|fn_name }}({%- call macros::param_list(func) %});
    {%- endmatch %}
    {%- endfor %}
}
