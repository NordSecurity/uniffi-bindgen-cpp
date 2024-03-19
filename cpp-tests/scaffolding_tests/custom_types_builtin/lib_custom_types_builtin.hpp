#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

#define DEFINE_CUSTOM_TYPE(custom, underlying)                      \
    typedef underlying custom;                                      \
    struct UniffiCustomTypeConverter##custom {                      \
        static custom into_custom(underlying val) { return val; }   \
        static underlying from_custom(custom val) { return val; }   \
    };

namespace {
    namespace custom_types_builtin {
        typedef std::unordered_map<std::string, std::string> StringMap;

        DEFINE_CUSTOM_TYPE(MyString, std::string)
        DEFINE_CUSTOM_TYPE(CustomString, std::string)
        DEFINE_CUSTOM_TYPE(Array, std::vector<std::string>)
        DEFINE_CUSTOM_TYPE(Bytes, std::vector<uint8_t>)
        DEFINE_CUSTOM_TYPE(Table, StringMap)
        DEFINE_CUSTOM_TYPE(Boolean, bool)
        DEFINE_CUSTOM_TYPE(Int8, int8_t)
        DEFINE_CUSTOM_TYPE(Int16, int16_t)
        DEFINE_CUSTOM_TYPE(Int32, int32_t)
        DEFINE_CUSTOM_TYPE(Int64, int64_t)
        DEFINE_CUSTOM_TYPE(UInt8, uint8_t)
        DEFINE_CUSTOM_TYPE(UInt16, uint16_t)
        DEFINE_CUSTOM_TYPE(UInt32, uint32_t)
        DEFINE_CUSTOM_TYPE(UInt64, uint64_t)
        DEFINE_CUSTOM_TYPE(Float, float)
        DEFINE_CUSTOM_TYPE(Double, double)

        struct CustomTypesBuiltin {
            MyString string;
            CustomString custom_string;
            Array array;
            Bytes bytes;
            Table table;
            Boolean boolean;
            Int8 int8;
            Int16 int16;
            Int32 int32;
            Int64 int64;
            UInt8 uint8;
            UInt16 uint16;
            UInt32 uint32;
            UInt64 uint64;
            Float flt;
            Double dbl;
        };

        CustomTypesBuiltin get_custom_types_builtin();
        CustomTypesBuiltin return_custom_types_builtin(CustomTypesBuiltin type);
    }
}
