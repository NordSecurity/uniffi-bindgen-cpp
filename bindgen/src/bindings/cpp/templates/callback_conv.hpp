{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}

struct {{ ffi_converter_name|class_name }} {
    static std::shared_ptr<{{ class_name }}> lift(uint64_t);
    static uint64_t lower(std::shared_ptr<{{ class_name }}>);
    static std::shared_ptr<{{ class_name }}> read(uniffi::RustStream &);
    static void write(RustStream &, uint64_t);

    static int32_t allocation_size(const {{ class_name }} &);

private:
    static int callback_stub(uint64_t, uint32_t, uint8_t *, int32_t, RustBuffer *);

    static std::once_flag once;
    static HandleMap<{{ class_name }}> callbacks;
};
