#include <cstdint>
#include <optional>
#include <string>

namespace {
    namespace custom_types {
        typedef std::string ExampleCustomType;
        struct UniffiCustomTypeConverterExampleCustomType {
            static ExampleCustomType into_custom(std::string val) {
                return val;
            }

            static std::string from_custom(ExampleCustomType val) {
                return val;
            }
        };

        typedef std::string Url;
        struct UniffiCustomTypeConverterUrl {
            static Url into_custom(std::string val) {
                return val;
            }

            static std::string from_custom(Url val) {
                return val;
            }
        };

        typedef int64_t Handle;
        struct UniffiCustomTypeConverterHandle {
            static Handle into_custom(int64_t val) {
                return val;
            }

            static int64_t from_custom(Handle val) {
                return val;
            }
        };

        typedef int64_t TimeIntervalMs;
        struct UniffiCustomTypeConverterTimeIntervalMs {
            static TimeIntervalMs into_custom(int64_t val) {
                return val;
            }

            static int64_t from_custom(TimeIntervalMs val) {
                return val;
            }
        };

        typedef double TimeIntervalSecDbl;
        struct UniffiCustomTypeConverterTimeIntervalSecDbl {
            static TimeIntervalSecDbl into_custom(double val) {
                return val;
            }

            static double from_custom(TimeIntervalSecDbl val) {
                return val;
            }
        };

        typedef float TimeIntervalSecFlt;
        struct UniffiCustomTypeConverterTimeIntervalSecFlt {
            static TimeIntervalSecFlt into_custom(float val) {
                return val;
            }

            static float from_custom(TimeIntervalSecFlt val) {
                return val;
            }
        };

        struct CustomTypesDemo {
            Url url;
            Handle handle;
            TimeIntervalMs time_interval_ms;
            TimeIntervalSecDbl time_interval_sec_dbl;
            TimeIntervalSecFlt time_interval_sec_flt;
        };

        CustomTypesDemo get_custom_types_demo(std::optional<CustomTypesDemo> demo);
        ExampleCustomType get_example_custom_type();
    }
}
