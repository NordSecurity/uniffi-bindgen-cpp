{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let type_name = typ|type_name %}
{%- let class_name = type_name|class_name %}
{%- let canonical_type_name = typ|canonical_name %}
struct {{ ffi_converter_name|class_name }} {
    static {{ type_name }} lift(uint64_t);
    static uint64_t lower(const {{ type_name }} &);
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, const {{ type_name }} &);
    static int32_t allocation_size(const {{ type_name }} &);

    static std::atomic<uint64_t> fn_handle;
};
