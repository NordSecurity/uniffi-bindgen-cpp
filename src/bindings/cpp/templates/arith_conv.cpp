{{ type_name }} {{ namespace }}::uniffi::{{ ffi_converter_name }}::lift({{ type_name }} val) {
    return val;
}

{{ type_name }} {{ namespace }}::uniffi::{{ ffi_converter_name }}::lower({{ type_name }} val) {
    return val;
}

{{ type_name }} {{ namespace }}::uniffi::{{ ffi_converter_name }}::read({{ namespace }}::uniffi::RustStream &stream) {
    {{ type_name }} ret;

    stream >> ret;

    return ret;
}

void {{ namespace }}::uniffi::{{ ffi_converter_name }}::write({{ namespace }}::uniffi::RustStream &stream, {{ type_name }} val) {
    stream << val;
}

int32_t {{ namespace }}::uniffi::{{ ffi_converter_name }}::allocation_size({{ type_name }}) {
    return sizeof({{ type_name }});
}
