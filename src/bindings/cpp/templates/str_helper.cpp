struct FfiConverterString {
    static std::string lift(RustBuffer buf) {
        auto stream = RustStream(&buf);
        auto string = read(stream);

        rustbuffer_free(buf);

        return string;
    }

    static RustBuffer lower(const std::string &val) {
        auto len = static_cast<int32_t>(val.length());
        auto bytes = ForeignBytes { len };

        val.copy(reinterpret_cast<char *>(bytes.data), len);

        return rustbuffer_from_bytes(bytes);
    }

    static std::string read(RustStream &stream) {
        int32_t len;
        std::string string;

        stream >> len;

        string.reserve(len);

        stream.get(reinterpret_cast<uint8_t *>(string.data()), len);

        return string;
    }

    static int32_t allocation_size(const std::string &val) {
        return sizeof(int32_t) + val.length();
    }
};
