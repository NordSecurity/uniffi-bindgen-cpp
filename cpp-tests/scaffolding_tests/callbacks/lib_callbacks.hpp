#include <stdexcept>
#include <string>
#include <memory>

namespace {
    namespace callbacks {
        namespace telephone_error {
            class Busy : public std::runtime_error {
            public:
                Busy() : std::runtime_error("I'm busy") {}
            };

            class InternalTelephoneError : public std::runtime_error {
            public:
                InternalTelephoneError() : std::runtime_error("Internal telephone error") {}
            };
        }

        class CallAnswerer {
        public:
            virtual std::string answer() = 0;

            virtual ~CallAnswerer() = default;
        };

        class Telephone {
        public:
            Telephone() = default;

            std::string call(std::shared_ptr<CallAnswerer> answerer);
        };
    }
}
