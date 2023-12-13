#include <test_common.hpp>

#include <custom_types_builtin.hpp>

void assert_custom_type(custom_types_builtin::CustomTypesBuiltin& type) {
    auto table = std::unordered_map<std::string, std::string>{{"hello", "world"}};
    auto bytes = std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    ASSERT_EQ(type.string, "Hello, world!");
    ASSERT_EQ(type.custom_string.string, "Custom string");
    ASSERT_EQ(type.array, std::vector<std::string>{"Hello, world!"});
    ASSERT_EQ(type.bytes, bytes);
    ASSERT_EQ(type.table, table);
    ASSERT_EQ(type.boolean, true);
    ASSERT_EQ(type.int8, std::numeric_limits<int8_t>::max());
    ASSERT_EQ(type.int16, std::numeric_limits<int16_t>::max());
    ASSERT_EQ(type.int32, std::numeric_limits<int32_t>::max());
    ASSERT_EQ(type.int64, std::numeric_limits<int64_t>::max());
    ASSERT_EQ(type.uint8, std::numeric_limits<uint8_t>::max());
    ASSERT_EQ(type.uint16, std::numeric_limits<uint16_t>::max());
    ASSERT_EQ(type.uint32, std::numeric_limits<uint32_t>::max());
    ASSERT_EQ(type.uint64, std::numeric_limits<uint64_t>::max());
    ASSERT_EQ(type.flt, std::numeric_limits<float>::max());
    ASSERT_EQ(type.dbl, std::numeric_limits<double>::max());
}

int main() {
    auto custom_type = custom_types_builtin::get_custom_types_builtin();
    assert_custom_type(custom_type);

    custom_type = custom_types_builtin::return_custom_types_builtin(custom_type);
    assert_custom_type(custom_type);

    return 0;
}