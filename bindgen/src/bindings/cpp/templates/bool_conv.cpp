{%- let class_name = ffi_converter_name|class_name %}

namespace {{ namespace }} {
    bool uniffi::{{ class_name }}::lift(uint8_t val) {
        return !!val;
    }

    uint8_t uniffi::{{ class_name }}::lower(bool val) {
        return val;
    }

    {{ type_name }} uniffi::{{ class_name }}::read(uniffi::RustStream &stream) {
        uint8_t val;

        stream.get(val);

        return val;
    }

    void uniffi::{{ class_name }}::write(uniffi::RustStream &stream, bool val) {
        stream.put(static_cast<uint8_t>(val));
    }

    int32_t uniffi::{{ class_name }}::allocation_size(bool) {
        return 1;
    }
}
