{%- let class_name = ffi_converter_name|class_name %}
bool {{ class_name }}::lift(uint8_t val) {
    return !!val;
}

uint8_t {{ class_name }}::lower(bool val) {
    return val;
}

{{ type_name }} {{ class_name }}::read(RustStream &stream) {
    uint8_t val;
    stream >> val;

    return val;
}

void {{ class_name }}::write(RustStream &stream, bool val) {
    stream << val;
}

int32_t {{ class_name }}::allocation_size(bool) {
    return 1;
}