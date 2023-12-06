{%- import "macros.cpp" as macros %}

{%- for typ in ci.iter_types() %}
{%- let type_name = typ|type_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let contains_object_references = ci.item_contains_object_references(typ) %}
{%- let namespace = ci.namespace() %}

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
{%- when Type::String %}
{% include "str_conv.cpp" %}
{%- else %}
{%- endmatch %}
{%- endfor %}