{%- let obj = ci.get_object_definition(name).unwrap() %}
{%- let (interface_name, impl_class_name) = obj|object_names %}

{%- let is_error = ci.is_name_used_as_error(name) %}
{%- if is_error %}
{{ type_name }} {{ typ|ffi_error_converter_name }}::lift(RustBuffer buf) {
    auto stream = RustStream(&buf);
    auto val = {{ ffi_converter_name }}::read(stream);
    rustbuffer_free(buf);

    return val;
}
{% endif %}

{{ type_name }} {{ ffi_converter_name }}::lift(uint64_t handle) {
    {%- if obj.has_callback_interface() %}
    // This interface can be implemented on either side of the FFI.
    // Rust makes its handles by leaking a pointer, so they are always even.
    if ((handle & 1) == 0) {
        return {{ type_name }}(new {{ impl_class_name }}(handle));
    }

    auto impl = handle_map.at(handle);
    handle_map.erase(handle);

    return impl;
    {%- else %}
    return {{ type_name }}(new {{ impl_class_name }}(handle));
    {%- endif %}
}

uint64_t {{ ffi_converter_name }}::lower(const {{ type_name }} &obj) {
    {%- if obj.is_trait_interface() %}
    // Rust values are instances of the generated implementation and own a handle we
    // can clone. Anything else is a C++ implementation of the trait.
    if (auto impl = std::dynamic_pointer_cast<{{ impl_class_name }}>(obj)) {
        return impl->_uniffi_internal_clone_handle();
    }
    {%- if obj.has_callback_interface() %}

    // The trait is `WithForeign`, so Rust can call back through the vtable using a handle of
    // our own.
    return handle_map.insert(obj);
    {%- else %}

    // Without a vtable there is no way to represent a C++ implementation, so report it rather than reinterpreting the pointer.
    throw std::runtime_error(
        "cannot pass this implementation of {{ interface_name }} to Rust: "
        "only values returned from Rust can be lowered");
    {%- endif %}
    {%- else %}
    return reinterpret_cast<{{ impl_class_name}}*>(obj.get())->_uniffi_internal_clone_handle();
    {%- endif %}
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    uint64_t ptr;
    stream >> ptr;

    return {{ ffi_converter_name }}::lift(ptr);
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &obj) {
    stream << {{ ffi_converter_name }}::lower(obj);
}

uint64_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &) {
    return 8;
}
