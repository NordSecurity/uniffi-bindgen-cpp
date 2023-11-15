{%- import "macros.cpp" as macros -%}

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

{%- when Type::Enum { name, module_path } %}
{%- let e = ci|get_enum_definition(name) %}
{%- if ci.is_name_used_as_error(name) %}
{% include "err_tmpl.cpp" %}
{%- else %}
{%- include "enum_tmpl.cpp" %}
{%- endif %}

{%- when Type::Object { module_path, name, imp } %}
{% include "obj.cpp" %}

{%- when Type::Record { module_path, name } %}
{% include "rec.cpp" %}

{%- when Type::Optional { inner_type } %}
{% include "opt_tmpl.cpp" %}

{%- when Type::Sequence { inner_type } %}
{% include "seq_tmpl.cpp" %}

{%- when Type::Map { key_type, value_type } %}
{% include "map_tmpl.cpp" %}

{%- when Type::CallbackInterface { module_path, name } %}
{% include "callback_iface_tmpl.cpp" %}

{%- when Type::Timestamp %}
{% include "timestamp_helper.cpp" %}

{%- when Type::Duration %}
{% include "duration_helper.cpp" %}

{%- when Type::External { module_path, name, namespace, kind, tagged } %}
{% include "ext_typ_tmpl.cpp" %}

{%- else %}
{%- endmatch %}
{%- endfor %}
