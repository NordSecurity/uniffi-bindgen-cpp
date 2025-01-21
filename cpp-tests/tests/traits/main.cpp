#include <test_common.hpp>

#include <traits.hpp>

struct CppButton : public traits::Button {
    public:
        virtual std::string name() override {
            return "C++ Button";
        }
};

int main() {
    auto buttons = traits::get_buttons();
    auto expected_names = {"go", "stop"};

    for (auto button : buttons) {
        auto name = button->name();

        ASSERT_TRUE(std::find(expected_names.begin(), expected_names.end(), name) != expected_names.end());
        ASSERT_EQ(traits::press(button)->name(), name);
    }

    auto native_button = std::make_shared<CppButton>();
    ASSERT_EQ(traits::press(native_button)->name(), "C++ Button");

    return 0;
}
