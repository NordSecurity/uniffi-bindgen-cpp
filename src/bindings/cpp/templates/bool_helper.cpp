namespace {{ ci.namespace() }}::uniffi {
    struct FfiConverterBool {
        static bool lift(uint8_t val) {
            return !!val;
        }

        static uint8_t lower(bool val) {
            return val;
        }

        static std::size_t allocation_size() {
            return 1;
        }
    };
}
