struct {{ ffi_converter_name }} {
    static {{ type_name }} lift({{ type_name }} val) {
        return val;
    }

    static {{ type_name }} lower({{ type_name }} val) {
        return val;
    }

    static {{ type_name }} read(RustStream &stream) {
        {{ type_name }} ret {};

        stream.read(reinterpret_cast<uint8_t *>(&ret), allocation_size(ret));

        if (std::endian::native != std::endian::big) {
            ret = byteswap(ret);
        }

        return ret;
    }

    static void write(RustStream &stream, {{ type_name }} val) {
        if (std::endian::native != std::endian::big) {
            val = byteswap(val);
        }

        stream.write(reinterpret_cast<uint8_t *>(&val), allocation_size(val));
    }

    static int32_t allocation_size({{ type_name }}) {
        return sizeof({{ type_name }});
    }
private:
    static {{ type_name }} byteswap({{ type_name }} val) {
        auto bytes = reinterpret_cast<char *>(&val);

        std::reverse(bytes, bytes + sizeof({{ type_name }}));

        return *reinterpret_cast<{{ type_name }} *>(bytes);
    }
};
