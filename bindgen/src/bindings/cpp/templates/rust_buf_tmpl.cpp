RustBuffer rustbuffer_alloc(int32_t len) {
    RustCallStatus status = { 0 };
    auto buffer = {{ ci.ffi_rustbuffer_alloc().name() }}(len, &status);

    check_rust_call(status, nullptr);

    return buffer;
}

RustBuffer rustbuffer_from_bytes(const ForeignBytes &bytes) {
    RustCallStatus status = { 0 };
    auto buffer = {{ ci.ffi_rustbuffer_from_bytes().name() }}(bytes, &status);

    check_rust_call(status, nullptr);

    return buffer;
}

void rustbuffer_free(RustBuffer buf) {
    RustCallStatus status = { 0 };

    {{ ci.ffi_rustbuffer_free().name() }}(std::move(buf), &status);
    check_rust_call(status, nullptr);
}