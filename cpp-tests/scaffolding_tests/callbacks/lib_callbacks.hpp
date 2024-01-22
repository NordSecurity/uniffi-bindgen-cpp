#include <string>

namespace lib_callbacks {
    struct CallAnswerer {
        virtual std::string answer() = 0;

        virtual ~CallAnswerer() = default;
    };
}