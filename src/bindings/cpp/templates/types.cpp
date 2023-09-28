namespace {{ ci.namespace() }}::uniffi {
{%- for type_ in ci.iter_types() %}
{%- let type_name = type_|type_name %}
{%- let ffi_converter_name = type_|ffi_converter_name %}
{%- let canonical_type_name = type_|canonical_name %}
{%- let contains_object_references = ci.item_contains_object_references(type_) %}
{%- let namespace = ci.namespace() %}

{%- match type_ %}
{%- when Type::Boolean %}
{% include "bool_helper.cpp" %}

{%- when Type::UInt8 %}
{% include "integral_helper.cpp" %}

{%- when Type::Int8 %}
{% include "integral_helper.cpp" %}

{%- when Type::UInt16 %}
{% include "integral_helper.cpp" %}

{%- when Type::Int16 %}
{% include "integral_helper.cpp" %}

{%- when Type::UInt32 %}
{% include "integral_helper.cpp" %}

{%- when Type::Int32 %}
{% include "integral_helper.cpp" %}

{%- when Type::UInt64 %}
{% include "integral_helper.cpp" %}

{%- when Type::Int64 %}
{% include "integral_helper.cpp" %}

{%- when Type::Float32 %}
{% include "integral_helper.cpp" %}

{%- when Type::Float64 %}
{% include "integral_helper.cpp" %}

{%- when Type::String %}
{% include "str_helper.cpp" %}

{%- when Type::Enum(name) %}
{%- let e = ci.get_enum_definition(name).unwrap() %}
{%- if ci.is_name_used_as_error(name) %}
{% include "err_tmpl.cpp" %}
{%- else %}
{%- include "enum_tmpl.cpp" %}
{%- endif %}

{%- when Type::Object { name, imp } %}
{% include "obj_tmpl.cpp" %}

{%- when Type::Record(name) %}
{% include "rec_conv.cpp" %}

{%- when Type::Optional(inner_type) %}
{% include "opt_tmpl.cpp" %}

{%- when Type::Sequence(inner_type) %}
{% include "seq_tmpl.cpp" %}

{%- when Type::Map(key_type, value_type) %}
{% include "map_tmpl.cpp" %}

{%- when Type::CallbackInterface(name) %}
{% include "callback_iface_tmpl.cpp" %}

{%- when Type::Timestamp %}
{% include "timestamp_helper.cpp" %}

{%- when Type::Duration %}
{% include "duration_helper.cpp" %}

{%- when Type::Custom { name, builtin } %}
{% include "cst_typ_tmpl.cpp" %}

{%- when Type::External { crate_name, name, kind } %}
{% include "ext_typ_tmpl.cpp" %}

{%- else %}
{%- endmatch %}
{%- endfor %}
}
