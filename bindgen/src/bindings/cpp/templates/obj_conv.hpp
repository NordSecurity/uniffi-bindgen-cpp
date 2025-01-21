{%- let type_name = typ|type_name %}
{%- let obj = ci|get_object_definition(name) %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let trait_impl=format!("UniffiCallbackInterface{}", canonical_type_name) %}
struct {{ typ|ffi_converter_name }} {
    static {{ type_name }} lift(void *);
    static void *lower(const {{ type_name }} &);
    static {{ type_name }} read(RustStream &);
    static void write(RustStream &, const {{ type_name }} &);
    static int32_t allocation_size(const {{ type_name }} &);
private:
    {%- if obj.has_callback_interface() %}
    friend struct {{ trait_impl }};
    inline static HandleMap<{{ canonical_type_name }}> handle_map = {};
    {%- endif %}
};
