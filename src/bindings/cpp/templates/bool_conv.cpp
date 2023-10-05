static bool ::{{ namespace }}::uniffi::{{ ffi_converter_name|class_name }}::lift(uint8_t val) {
    return !!val;
}

static uint8_t ::{{ namespace }}::uniffi::{{ ffi_converter_name|class_name }}::lower(bool val) {
    return val;
}

static {{ type_name }} ::{{ namespace }}::uniffi::{{ ffi_converter_name|class_name }}::read(::{{ namespace }}::uniffi::RustStream &stream) {
    uint8_t val;

    stream.get(val);

    return val;
}

static void ::{{ namespace }}::uniffi::{{ ffi_converter_name|class_name }}::write(::{{ namespace }}::uniffi::RustStream &stream, bool val) {
    stream.put(static_cast<uint8_t>(val));
}

static int32_t ::{{ namespace }}::uniffi::{{ ffi_converter_name|class_name }}::allocation_size(const bool &) {
    return 1;
}
