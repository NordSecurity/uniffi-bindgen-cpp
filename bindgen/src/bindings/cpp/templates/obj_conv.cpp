{%- let obj = ci|get_object_definition(name) %}
{%- let (interface_name, impl_class_name) = obj|object_names %}

{{ type_name }} {{ ffi_converter_name }}::lift(void *ptr) {
    return {{ type_name }}(new {{ impl_class_name }}(ptr));
}

void *{{ ffi_converter_name }}::lower(const {{ type_name }} &obj) {
    {%- if obj.has_callback_interface() %}
    auto ptr = handle_map.insert(obj);
    return reinterpret_cast<void*>(ptr);
    {%- else %}
    return reinterpret_cast<{{ impl_class_name}}*>(obj.get())->uniffi_clone_pointer();
    {%- endif %}
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    std::uintptr_t ptr;
    stream >> ptr;

    return {{ type_name }}(new {{ impl_class_name }}(reinterpret_cast<void *>(ptr)));
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &obj) {
    stream << reinterpret_cast<std::uintptr_t>(reinterpret_cast<{{ impl_class_name }}*>(obj.get())->uniffi_clone_pointer());
}

int32_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &) {
    return 8;
}
