struct RustStreamBuffer: std::basic_streambuf<uint8_t> {
    RustStreamBuffer(RustBuffer *buf) {
        this->setg(buf->data, buf->data, buf->data + buf->len);
        this->setp(buf->data, buf->data + buf->capacity);
    }
    ~RustStreamBuffer() = default;

private:
    RustStreamBuffer() = delete;
    RustStreamBuffer(const RustStreamBuffer &) = delete;
    RustStreamBuffer(RustStreamBuffer &&) = delete;

    RustStreamBuffer &operator=(const RustStreamBuffer &) = delete;
    RustStreamBuffer &operator=(RustStreamBuffer &&) = delete;
};

struct RustStream: std::basic_iostream<uint8_t> {
    RustStream(RustBuffer *buf):
        streambuf(RustStreamBuffer(buf)), std::basic_iostream<uint8_t>(&streambuf) { }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    RustStream &operator>>(T &val) {
        read(reinterpret_cast<uint8_t *>(&val), sizeof(T));

        if (std::endian::native != std::endian::big) {
            auto bytes = reinterpret_cast<char *>(&val);

            std::reverse(bytes, bytes + sizeof(T));
        }

        return *this;
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    RustStream &operator<<(T val) {
        if (std::endian::native != std::endian::big) {
            auto bytes = reinterpret_cast<char *>(&val);

            std::reverse(bytes, bytes + sizeof(T));
        }

        write(reinterpret_cast<uint8_t *>(&val), sizeof(T));

        return *this;
    }
private:
    RustStreamBuffer streambuf;
};

