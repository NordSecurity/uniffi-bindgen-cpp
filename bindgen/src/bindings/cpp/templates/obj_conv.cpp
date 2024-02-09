{{ type_name }} {{ ffi_converter_name }}::lift(void *ptr) {
    return {{ type_name }}(new {{ canonical_type_name }}(ptr));
}

void *{{ ffi_converter_name }}::lower(const {{ type_name }} &obj) {
    return obj->instance;
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    std::uintptr_t ptr;
    stream >> ptr;

    return {{ type_name }}(new {{ canonical_type_name }}(reinterpret_cast<void *>(ptr)));
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &obj) {
    stream << reinterpret_cast<std::uintptr_t>(obj->instance);
}

int32_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &) {
    return 8;
}