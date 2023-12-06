struct {{ typ|ffi_converter_name }} {
    static std::string lift(RustBuffer buf);
    static RustBuffer lower(const std::string &);
    static std::string read(RustStream &);
    static void write(RustStream &, const std::string &);
    static int32_t allocation_size(const std::string &);
};