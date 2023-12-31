{%- let obj = ci|get_object_definition(name) %}
{%- let class_name = type_name|class_name %}
{%- let ffi_converter_name = typ|ffi_converter_name %}
{%- let canonical_type_name = typ|canonical_name %}
{%- let type_name = typ|type_name %}
namespace uniffi {
    struct {{ ffi_converter_name|class_name }};
} // namespace uniffi

{%~ call macros::docstring(obj, 0) %}
struct {{ canonical_type_name }} {
    friend uniffi::{{ ffi_converter_name|class_name }};

    {{ canonical_type_name }}() = delete;

    {{ canonical_type_name }}(const {{ canonical_type_name }} &) = delete;
    {{ canonical_type_name }}({{ canonical_type_name }} &&) = delete;

    {{ canonical_type_name }} &operator=(const {{ canonical_type_name }} &) = delete;
    {{ canonical_type_name }} &operator=({{ canonical_type_name }} &&) = delete;

    ~{{ canonical_type_name }}();
    
    {%- match obj.primary_constructor() %}
    {%- when Some with (ctor) %}
    {%- call macros::docstring(ctor, 4) %}
    static {{ type_name }} init({% call macros::param_list(ctor) %});
    {%- else %}
    {%- endmatch %}

    {%- for ctor in obj.alternate_constructors() %}
    {%- call macros::docstring(ctor, 4) %}
    static {{ type_name }} {{ ctor.name() }}({% call macros::param_list(ctor) %});
    {%- endfor %}

    {%- for method in obj.methods() %}
    {%- call macros::docstring(method, 4) %}
    {% match method.return_type() %}{% when Some with (return_type) %}{{ return_type|type_name }} {% else %}void {% endmatch %}
    {{- method.name()|fn_name }}({% call macros::param_list(method) %});
    {%- endfor %}

    {%- for method in obj.uniffi_traits() %}
    {%- match method %}
    {%- when UniffiTrait::Display { fmt } %}
    /**
     * Returns a string representation of the object, internally calls Rust's `Display` trait. 
     */
    std::string to_string() const;
    {%- when UniffiTrait::Debug { fmt } %}
    /**
     * Returns a string representation of the object, internally calls Rust's `Debug` trait. 
     */
    std::string to_debug_string() const;
    {%- when UniffiTrait::Eq { eq, ne } %}
    /**
     * Equality check, internally calls Rust's `Eq` trait. 
     */
    bool eq(const {{ type_name }} &other) const;
    /**
     * Inequality check, internally calls Rust's `Ne` trait. 
     */
    bool ne(const {{ type_name }} &other) const;
    {%- when UniffiTrait::Hash { hash } %}
    /**
     * Returns a hash of the object, internally calls Rust's `Hash` trait. 
     */
    uint64_t hash() const;
    {%- endmatch %}
    {%- endfor %}

private:
    {{ canonical_type_name }}(void *);

    void *instance;
};