RustBuffer {{ ci.namespace() }}::uniffi::rustbuffer_alloc(int32_t len) {
    RustCallStatus status = { 0 };
    auto buffer = {{ ci.ffi_rustbuffer_alloc().name() }}(len, &status);

    check_rust_call(&status, nullptr);

    return buffer;
}

void {{ ci.namespace() }}::uniffi::rustbuffer_free(RustBuffer buf) {
    RustCallStatus status = { 0 };

    {{ ci.ffi_rustbuffer_free().name() }}(std::move(buf), &status);
    check_rust_call(&status, nullptr);
}
