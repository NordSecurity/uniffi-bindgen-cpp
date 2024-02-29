#include <test_common.hpp>

#include <traits.hpp>

int main() {
    auto buttons = traits::get_buttons();
    auto expected_names = {"go", "stop"};

    for (auto button : buttons) {
        auto name = button->name();

        ASSERT_TRUE(std::find(expected_names.begin(), expected_names.end(), name) != expected_names.end());
        ASSERT_EQ(traits::press(button)->name(), name);
    }

    return 0;
}
