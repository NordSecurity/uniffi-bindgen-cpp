#include <test_common.hpp>

#include <custom_fixture_callbacks.hpp>

struct ForeignGetters : public custom_fixture_callbacks::ForeignGetters {
public:
    virtual bool get_bool(bool v, bool arg2) override {
        return v ^ arg2;
    }

    virtual std::string get_string(std::string v, bool arg2) override {
        return arg2 ? "1234567890123" : v;
    }

    virtual std::vector<int32_t> get_list(std::vector<int32_t> v, bool arg2) override {
        return arg2 ? v : std::vector<int32_t>();
    }

    virtual std::optional<std::string> get_option(std::optional<std::string> v, bool arg2) override {
        return arg2 ? std::optional<std::string>("1234567890123") : v;
    }

    virtual std::vector<uint8_t> get_bytes(std::vector<uint8_t> v, bool arg2) override {
        return arg2 ? v : std::vector<uint8_t>();
    }
};

struct CppStringifier {
    std::string from_simple_type(int32_t value) { return "C++: " + std::to_string(value); }

    std::string from_complex_type(std::optional<std::vector<std::optional<double>>> values) {
        if (values.has_value()) {
            std::string result = "C++:";
            for (auto &v : values.value()) {
                if (v.has_value()) {
                    result += std::to_string(v.value());
                } else {
                    result += "null";
                }
                result += ",";
            }

            if (values.value().size() > 0) {
                result.pop_back();
            }

            return result;
        } else {
            return "C++: null";
        }
    }
};

int main() {
    auto cb = std::make_shared<ForeignGetters>();

    auto flag = true;

    ASSERT_EQ(cb->get_string("test", flag), custom_fixture_callbacks::get_string_roundtrip(cb, "test", flag));
    ASSERT_EQ(cb->get_bool(true, flag), custom_fixture_callbacks::get_bool_roundtrip(cb, true, flag));
    ASSERT_EQ(cb->get_bool(false, flag), custom_fixture_callbacks::get_bool_roundtrip(cb, false, flag));

    for (auto list : {std::vector<int32_t>{1, 2}, std::vector<int32_t>{0, 1}}) {
        ASSERT_EQ(cb->get_list(list, flag), custom_fixture_callbacks::get_list_roundtrip(cb, list, flag));
    }

    for (auto str : {"Hello", "World"}) {
        ASSERT_EQ(cb->get_string(str, flag), custom_fixture_callbacks::get_string_roundtrip(cb, str, flag));
    }

    for (auto opt : {std::optional<std::string>("Hello"), std::optional<std::string>(std::nullopt)}) {
        ASSERT_EQ(cb->get_option(opt, flag), custom_fixture_callbacks::get_option_roundtrip(cb, opt, flag));
    }

    ASSERT_EQ("TestString", custom_fixture_callbacks::get_string_optional_callback(cb, "TestString", false));
    ASSERT_EQ(custom_fixture_callbacks::get_string_optional_callback(nullptr, "TestString", false), std::nullopt);

    for (auto bytes : {std::vector<uint8_t>{1, 2}, std::vector<uint8_t>{0, 1}}) {
        ASSERT_EQ(cb->get_bytes(bytes, flag), custom_fixture_callbacks::get_bytes_roundtrip(cb, bytes, flag));
    }

    auto stringifier = std::make_shared<CppStringifier>();
    ASSERT_EQ(stringifier->from_simple_type(123), custom_fixture_callbacks::stringify_simple(123));

    for (auto num : { std::optional<std::vector<std::optional<double>>>(), std::optional<std::vector<std::optional<double>>>(std::vector<std::optional<double>>{std::nullopt, 1.0, 2.0}) }) {
        ASSERT_EQ(stringifier->from_complex_type(num), custom_fixture_callbacks::stringify_complex(num));
    }

    auto map = std::unordered_map<std::string, std::optional<std::string>>{
        {"key1", std::optional<std::string>("value1")},
        {"key2", std::optional<std::string>("value2")},
        {"key3", std::optional<std::string>(std::nullopt)},
    };

    for (auto &[key, val] : map) {
        ASSERT_EQ(val, custom_fixture_callbacks::roundtrip_record(map, key));
    }

    return 0;
}
