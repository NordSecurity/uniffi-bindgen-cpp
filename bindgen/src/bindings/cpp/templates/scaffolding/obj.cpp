{{ type_name }} {{ ffi_converter_name }}::lift(uint64_t ptr) {
    return {{ name }}_map.at((uint64_t)ptr);
}

uint64_t {{ ffi_converter_name }}::lower(const {{ type_name }} &obj) {
    auto ret = {{ name }}_map.insert(obj);
    return ret;
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    std::uint64_t ptr;
    stream >> ptr;

    return {{ name }}_map.at(ptr);
}

void {{ ffi_converter_name }}::write(RustStream &stream, const {{ type_name }} &obj) {
    {{ name }}_map.insert(obj);
    stream << (uint64_t)obj.get();
}

int32_t {{ ffi_converter_name }}::allocation_size(const {{ type_name }} &) {
    return 8;
}
