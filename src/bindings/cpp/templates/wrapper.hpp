#pragma once

#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <streambuf>

#include "{{ ci.namespace() }}_scaffolding.hpp"

namespace {{ ci.namespace() }}::uniffi {
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
    private:
        RustStreamBuffer streambuf;
    };

    RustBuffer rustbuffer_alloc(int32_t len);
    RustBuffer rustbuffer_from_bytes(const ForeignBytes &bytes);
    void rustbuffer_free(RustBuffer buf);
}

{%- import "macros.cpp" as macros %}

namespace {{ ci.namespace() }} {
    {%- for typ in self.sorted_records(ci.iter_types()) %}
    {%- match typ %}
    {%- when Type::Record(name) %}
    {% include "rec_tmpl.cpp" %}
    {% else %}
    {%- endmatch %}
    {%- endfor %}

    {%- for func in ci.function_definitions() %}
    {%- match func.return_type() %}
    {%- when Some with (return_type) %}
    {{ return_type|type_name }} {{ func.name()|fn_name }}({%- call macros::arg_list_decl(func) %});
    {%- when None %}
    void {{ func.name()|fn_name }}({%- call macros::arg_list_decl(func) %});
    {%- endmatch %}
    {%- endfor %}
}
