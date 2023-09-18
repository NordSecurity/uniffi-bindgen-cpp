namespace {{ ci.namespace() }}::uniffi {
    struct FfiConverterString {
        static std::string lift(RustBuffer buf) {
            auto stream = RustStream(&buf);
            auto string = FfiConverterString::read(stream);

            rustbuffer_free(buf);

            return string;
        }

        static RustBuffer *lower(const std::string &val) {
            return nullptr;
        }

        static std::string read(RustStream &stream) {
            std::string string;

            stream >> string;

            return string;
        }
    };
}
