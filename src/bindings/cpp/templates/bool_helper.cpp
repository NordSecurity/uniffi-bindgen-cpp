struct FfiConverterBool {
    static bool lift(uint8_t val) {
        return !!val;
    }

    static uint8_t lower(bool val) {
        return val;
    }

    static {{ type_name }} read(RustStream &stream) {
        uint8_t val;

        stream >> val;

        return val;
    }

    static void write(RustStream &stream, bool val) {
        stream << static_cast<uint8_t>(val);
    }

    static int32_t allocation_size(bool) {
        return 1;
    }
};
