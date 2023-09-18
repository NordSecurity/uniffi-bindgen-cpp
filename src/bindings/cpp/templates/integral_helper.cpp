namespace {{ ci.namespace() }}::uniffi {
    struct {{ ffi_converter_name }} {
        static {{ type_name }} lift({{ type_name }} val) {
            if (std::endian::native != std::endian::big) {
                return std::byteswap(val);
            }

            return val;
        }

        static {{ type_name }} lower({{ type_name }} val) {
            if (std::endian::native != std::endian::big) {
                return std::byteswap(val);
            }

            return val;
        }

        static std::size_t allocation_size() {
            return sizeof({{ type_name }});
        }
    };
}
