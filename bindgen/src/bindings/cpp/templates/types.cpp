{%- import "macros.cpp" as macros %}

{%- for typ in ci.iter_local_types() %}
{%- let type_name = typ|type_name(ci) %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let contains_object_references = ci.item_contains_object_references(typ) %}
{%- let namespace = ci.namespace() %}

{%- match typ %}
{%- when Type::Object { module_path, name, imp } %}
{% include "obj.cpp" %}
{%- when Type::Record { module_path, name } %}
{%- let rec = ci.get_record_definition(name).unwrap() %}
{%- let self_expr = format!("uniffi::{}::lower(*this)", ffi_converter_name) %}
{% call macros::method_defs(rec.methods(), type_name, self_expr, " const") %}
{% call macros::uniffi_trait_defs(rec.uniffi_trait_methods(), type_name, type_name, self_expr) %}
{%- when Type::Enum { module_path, name } %}
{%- let e = ci.get_enum_definition(name).unwrap() %}
{%- if ci.is_name_used_as_error(name) %}
{%- let class_name = typ|canonical_name %}
{%- let self_expr = format!("uniffi::{}::lower(*this)", ffi_converter_name) %}
{% call macros::method_defs(e.methods(), class_name, self_expr, " const") %}
{% call macros::uniffi_trait_defs(e.uniffi_trait_methods(), class_name, class_name, self_expr) %}
{%- else if e.is_flat() %}
{%- let self_expr = format!("uniffi::{}::lower(value)", ffi_converter_name) %}
{% call macros::method_free_defs(e.methods(), type_name, self_expr) %}
{% call macros::uniffi_trait_free_defs(e.uniffi_trait_methods(), type_name, self_expr) %}
{%- else %}
{%- let self_expr = format!("uniffi::{}::lower(*this)", ffi_converter_name) %}
{% call macros::method_defs(e.methods(), type_name, self_expr, " const") %}
{% call macros::uniffi_trait_defs(e.uniffi_trait_methods(), type_name, type_name, self_expr) %}
{%- endif %}
{%- else %}
{%- endmatch %}
{% endfor ~%}

namespace uniffi {
{%- for typ in ci.iter_local_types() %}
{%- let type_name = typ|type_name(ci) %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let contains_object_references = ci.item_contains_object_references(typ) %}
{%- let namespace = ci.namespace() %}

{%- match typ %}
{%- when Type::Enum { name, module_path } %}
{%- let e = ci.get_enum_definition(name).unwrap() %}
{%- if ci.is_name_used_as_error(name) %}
{% include "err_tmpl.cpp" %}
{%- else %}
{% include "enum_tmpl.cpp" %}
{%- endif %}
{%- when Type::Object { module_path, name, imp } %}
{% include "obj_conv.cpp" %}
{%- when Type::Record { module_path, name } %}
{% include "rec.cpp" %}
{%- when Type::Optional { inner_type } %}
{% include "opt_tmpl.cpp" %}
{%- when Type::Sequence { inner_type } %}
{% include "seq_tmpl.cpp" %}
{%- when Type::Map { key_type, value_type } %}
{% include "map_tmpl.cpp" %}
{%- when Type::CallbackInterface { module_path, name } %}
{%- let cbi = ci.get_callback_interface_definition(name).unwrap() %}
{%- let ffi_init_callback = cbi.ffi_init_callback() %}
{%- let interface_name = name %}
{%- let methods = cbi.methods() %}
{%- let vtable = cbi.vtable() %}
{%- let vtable_methods = cbi.vtable_methods() %}
{% include "callback_conv.cpp" %}
{% include "callback_iface_tmpl.cpp" %}
{%- when Type::Timestamp %}
{% include "timestamp_helper.cpp" %}
{%- when Type::Duration %}
{% include "duration_helper.cpp" %}
{%- when Type::Custom { module_path, name, builtin } %}
{%- include "custom.cpp" %}
{%- else %}
{%- endmatch %}
{% endfor ~%}

}
