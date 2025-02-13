{{ type_name }} {{ ffi_converter_name }}::lift({{ type_name }} val) {
    return val;
}

{{ type_name }} {{ ffi_converter_name }}::lower({{ type_name }} val) {
    return val;
}

{{ type_name }} {{ ffi_converter_name }}::read(RustStream &stream) {
    {{ type_name }} ret;
    stream >> ret;

    return ret;
}

void {{ ffi_converter_name }}::write(RustStream &stream, {{ type_name }} val) {
    stream << val;
}

uint64_t {{ ffi_converter_name }}::allocation_size({{ type_name }}) {
    return static_cast<uint64_t>(sizeof({{ type_name }}));
}
