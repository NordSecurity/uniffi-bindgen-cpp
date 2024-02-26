#include <memory>
#include <string>
#include <vector>

namespace {
    namespace traits {
        class Button {
        public:
            virtual ~Button() = default;
            virtual std::string name() = 0;
        };

        class GoButton: public Button {
        public:
            std::string name() override {
                return "go";
            }
        };

        class StopButton: public Button {
        public:
            std::string name() override {
                return "stop";
            }
        };

        std::vector<std::shared_ptr<Button>> get_buttons() {
            return {std::make_shared<GoButton>(), std::make_shared<StopButton>()};
        }

        std::shared_ptr<Button> press(std::shared_ptr<Button> button) {
            return button;
        }
    }
}
