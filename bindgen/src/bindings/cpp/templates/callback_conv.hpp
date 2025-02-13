{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let canonical_type_name = typ|canonical_name %}

struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(uint64_t);
    static uint64_t lower(const {{ type_name }} &);
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, const {{ type_name }} &);
    static int32_t allocation_size(const {{ type_name }} &);

    inline static HandleMap<{{ canonical_type_name }}> handle_map = {};
};
