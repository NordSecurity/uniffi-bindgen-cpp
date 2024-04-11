#include <cstdint>
#include <variant>

namespace {
    namespace enum_style_test {
        enum class SimpleEnum: int32_t {
            VARIANT_ONE = 1,
            VARIANT_TWO = 2,
            VARIANT_THREE = 3
        };


        class ComplexEnum {
        public:
            struct VARIANT_ONE { uint32_t num; };
            struct VARIANT_TWO { float flt; };

            ComplexEnum(ComplexEnum::VARIANT_ONE variant) { this->variant = variant; }
            ComplexEnum(ComplexEnum::VARIANT_TWO variant) { this->variant = variant; }

            std::variant<VARIANT_ONE, VARIANT_TWO> variant;
        };

        SimpleEnum get_simple_enum();
        void set_simple_enum(SimpleEnum e);

        ComplexEnum get_complex_enum();
        void set_complex_enum(ComplexEnum e);
    }
}
