#include <cstdint>

namespace {
    namespace empty_type {
        struct Empty {};

        Empty get_empty_type();

        void send_empty_type(Empty e);
    }
}
