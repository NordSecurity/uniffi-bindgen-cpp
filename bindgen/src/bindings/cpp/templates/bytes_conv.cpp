{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let class_name = ffi_converter_name|class_name %}
{%- let type_name = typ|type_name %}

namespace {{ namespace }} {
    {{ type_name }} uniffi::{{ class_name }}::lift(RustBuffer buf) {
        auto stream = RustStream(&buf);
        auto ret = read(stream);

        rustbuffer_free(buf);

        return ret;
    }

    RustBuffer uniffi::{{ class_name }}::lower(const {{ type_name }} &val) {
        auto buf = uniffi::rustbuffer_alloc(allocation_size(val));
        auto stream = RustStream(&buf);

        write(stream, val);

        return buf;
    }

    {{ type_name }} uniffi::{{ class_name }}::read(uniffi::RustStream &stream) {
        {{ type_name }} ret;
        int32_t count;

        stream >> count;

        ret.reserve(count);

        for (decltype(count) i = 0; i < count; i++) {
            uint8_t elem;
            stream >> elem;
            ret.push_back(elem);
        }

        return ret;
    }

    void uniffi::{{ class_name }}::write(uniffi::RustStream &stream, const {{ type_name }} &val) {
        stream << static_cast<int32_t>(val.size());

        for (auto &elem : val) {
            stream << elem;
        }
    }

    int32_t uniffi::{{ class_name }}::allocation_size(const {{ type_name }} &val) {
        return sizeof(int32_t) + sizeof(uint8_t) * val.size();
    }
}