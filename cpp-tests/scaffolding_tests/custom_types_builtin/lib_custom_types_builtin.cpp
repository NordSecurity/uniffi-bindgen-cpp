#include "lib_custom_types_builtin.hpp"

#include <limits>

custom_types_builtin::CustomTypesBuiltin custom_types_builtin::get_custom_types_builtin() {
    return custom_types_builtin::CustomTypesBuiltin{
        .string = "Hello, world!",
        .custom_string = custom_types_builtin::CustomString{"Custom string"},
        .array = std::vector<std::string>{"Hello, world!"},
        .bytes = std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        .table = std::unordered_map<std::string, std::string>{{"hello", "world"}},
        .boolean = true,
        .int8 = std::numeric_limits<int8_t>::max(),
        .int16 = std::numeric_limits<int16_t>::max(),
        .int32 = std::numeric_limits<int32_t>::max(),
        .int64 = std::numeric_limits<int64_t>::max(),
        .uint8 = std::numeric_limits<uint8_t>::max(),
        .uint16 = std::numeric_limits<uint16_t>::max(),
        .uint32 = std::numeric_limits<uint32_t>::max(),
        .uint64 = std::numeric_limits<uint64_t>::max(),
        .flt = std::numeric_limits<float>::max(),
        .dbl = std::numeric_limits<double>::max(),
    };
}

custom_types_builtin::CustomTypesBuiltin custom_types_builtin::return_custom_types_builtin(custom_types_builtin::CustomTypesBuiltin type) {
    return type;
}

#include <custom_types_builtin_cpp_scaffolding.cpp>
