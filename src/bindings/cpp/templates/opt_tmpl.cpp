struct {{ ffi_converter_name }} {
    static {{ type_name }} lift(RustBuffer buf) {
        auto stream = RustStream(&buf);
        auto ret = {{ ffi_converter_name }}::read(stream);

        rustbuffer_free(buf);

        return ret;
    }

    static RustBuffer lower({{ type_name }} val) {
        auto buf = rustbuffer_alloc(allocation_size(val));
        auto stream = RustStream(&buf);

        {{ ffi_converter_name }}::write(stream, val);

        return buf;
    }

    static {{ type_name }} read(RustStream &stream) {
        uint8_t has_value;

        stream.get(has_value);

        if (has_value) {
            return std::make_optional({{ inner_type|read_fn }}(stream));
        }

        std::cout << stream.good() << ' ' << stream.bad() << ' ' << stream.fail() << '\n';

        return std::nullopt;
    }

    static void write(RustStream &stream, {{ type_name }} value) {
        stream << static_cast<uint8_t>(!!value);

        if (value) {
            {{ inner_type|write_fn }}(stream, value.value());
        }
    }

    static int32_t allocation_size(const {{ type_name }} &val) {
        int32_t ret = 1;

        if (val) {
            ret += {{ inner_type|allocation_size_fn }}(val.value());
        }

        return ret;
    }
};
