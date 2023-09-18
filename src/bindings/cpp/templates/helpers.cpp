template<typename T, typename... Ts, typename... Us> void _check_rust_call(std::function<T(Ts...)> func, Us...) {
    
}
void check_rust_call(
    RustCallStatus *status,
    std::function<std::exception(RustBuffer)> error_cb
) {
    switch (status->code) {
    case 0:
        return;

    case 1:
        if (error_cb) {
            throw error_cb(status->error_buf);
        }
        break;

    case 2:
        if (status->error_buf.len > 0) {
            throw std::runtime_error({{ ci.namespace() }}::uniffi::{{ Type::String.borrow()|lift_fn }}(status->error_buf));
        }

        throw std::runtime_error("A Rust panic has occurred");
    }

    throw std::runtime_error("Unexpected Rust call status");
}
