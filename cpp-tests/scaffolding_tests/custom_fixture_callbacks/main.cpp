#include <test_common.hpp>

#include <custom_fixture_callbacks.hpp>

class ForeignGetters : public custom_fixture_callbacks::ForeignGetters {
public:
    virtual bool get_bool(bool v, bool arg2) override { return v ^ arg2; }

    virtual std::string get_string(std::string v, bool arg2) override { return arg2 ? "1234567890123" : v; }

    // virtual std::vector<int32_t> get_list(std::vector<int32_t> v, bool arg2) override { return arg2 ? std::vector<int32_t>{1, 2, 3} : v; }

    virtual std::optional<std::string> get_option(std::optional<std::string> v, bool arg2) override { return arg2 ? std::optional<std::string>("1234567890123") : v; }
};

int main() {
    auto cb = std::make_shared<ForeignGetters>();

    auto flag = true;

    ASSERT_EQ(cb->get_string("test", flag), custom_fixture_callbacks::get_string_roundtrip(cb, "test", flag));
    ASSERT_EQ(cb->get_bool(true, flag), custom_fixture_callbacks::get_bool_roundtrip(cb, true, flag));
    ASSERT_EQ(cb->get_bool(false, flag), custom_fixture_callbacks::get_bool_roundtrip(cb, false, flag));

    for (auto list : {std::vector<int32_t>{1, 2}, std::vector<int32_t>{0, 1}}) {
        // ASSERT_EQ(cb->get_list(list, flag), custom_fixture_callbacks::get_list_roundtrip(cb, list, flag));
    }

    for (auto str : {"Hello", "World"}) {
        ASSERT_EQ(cb->get_string(str, flag), custom_fixture_callbacks::get_string_roundtrip(cb, str, flag));
    }

    for (auto opt : {std::optional<std::string>("Hello"), std::optional<std::string>(std::nullopt)}) {
        ASSERT_EQ(cb->get_option(opt, flag), custom_fixture_callbacks::get_option_roundtrip(cb, opt, flag));
    }

    ASSERT_EQ("TestString", custom_fixture_callbacks::get_string_optional_callback(cb, "TestString", false));
    ASSERT_EQ(custom_fixture_callbacks::get_string_optional_callback(nullptr, "TestString", false), std::nullopt);

    return 0;
}
