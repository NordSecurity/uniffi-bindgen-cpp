{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let type_name = typ|type_name %}
{%- let canonical_type_name = typ|canonical_name %}
struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(uint64_t);
    static uint64_t lower(const {{ type_name }} &);
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, const {{ type_name }} &);
    static int32_t allocation_size(const {{ type_name }} &);
private:
    static int callback_stub(uint64_t, uint32_t, uint8_t *, int32_t, RustBuffer *);

    static std::once_flag once;
    static HandleMap<{{ canonical_type_name }}> callbacks;
};
