#include <stdint.h>
#include <stdexcept>

namespace {
    namespace arithmetic {
        namespace arithmetic_error {
            class IntegerOverflow: public std::runtime_error {
                public:
                    IntegerOverflow(): std::runtime_error("integer overflow") { }
            };
        };

        uint64_t add(uint64_t a, uint64_t b);
        uint64_t sub(uint64_t a, uint64_t b);
        uint64_t div(uint64_t a, uint64_t b);
        int8_t equal(uint64_t a, uint64_t b);
    }
}
