#include <cstdint>
{{ type_name }} {{ ffi_converter_name }}::lift(void *ptr) {
    return {{ name }}_map.at((uint64_t)ptr);
}

void *{{ ffi_converter_name }}::lower(const {{ type_name }} &obj) {
    return (void *)obj.get();
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    std::uintptr_t ptr;
    stream >> ptr;

    return {{ name }}_map.at(ptr);
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &obj) {
    stream << (uint64_t)obj.get();
}

int32_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &) {
    return 8;
}
