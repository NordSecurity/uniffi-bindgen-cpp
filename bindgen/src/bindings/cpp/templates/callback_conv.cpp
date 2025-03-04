{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let class_name = ffi_converter_name|class_name %}

{{ type_name }} {{ class_name }}::lift(uint64_t handle) {
    return handle_map.at(handle);
}

uint64_t {{ class_name }}::lower(const {{type_name}}& impl) {
    return handle_map.insert(impl);
}

{{ type_name }} uniffi::{{ class_name }}::read(RustStream &stream) {
    uint64_t handle;
    stream >> handle;

    return lift(handle);
}

void uniffi::{{ class_name }}::write(RustStream &stream, const {{ type_name }} &impl) {
    stream << lower(impl);
}

uint64_t uniffi::{{ class_name }}::allocation_size(const {{ type_name }} &impl) {
    return static_cast<uint64_t>(sizeof(uint64_t));
}
