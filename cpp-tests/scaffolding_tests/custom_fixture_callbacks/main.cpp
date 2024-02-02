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
        if (v.has_value()) {

            if (arg2) {
                std::transform(v.value().begin(), v.value().end(), v.value().begin(), [](char c) { return ::toupper(c); });
            }

            return v;
        }

        return std::nullopt;
    }

    virtual std::vector<uint8_t> get_bytes(std::vector<uint8_t> v, bool arg2) override {
        return arg2 ? v : std::vector<uint8_t>();
    }

    virtual void get_nothing(std::string v) override {
        // TODO errors
    }

    virtual custom_fixture_callbacks::Enumeration get_enum(custom_fixture_callbacks::Enumeration v, uint32_t variant, bool arg2) override {
        if (!arg2) {
            return v;
        }

        switch (variant) {
            case 0:
                return custom_fixture_callbacks::Enumeration::A;
            case 1:
                return custom_fixture_callbacks::Enumeration::B;
            case 2:
                return custom_fixture_callbacks::Enumeration::C;
            default:
                return custom_fixture_callbacks::Enumeration::UNKNOWN;
        }
    }
};

struct CppStringifier : public custom_fixture_callbacks::StoredForeignStringifier {
    std::string from_simple_type(int32_t value) override { return "C++: " + std::to_string(value); }

    std::string from_complex_type(std::optional<std::vector<std::optional<double>>> values) override {
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
    auto native_cb = custom_fixture_callbacks::NativeGetters::init();

    auto flag = true;

    ASSERT_EQ(cb->get_string("test", flag), native_cb->get_string(cb, "test", flag));
    ASSERT_EQ(cb->get_bool(true, flag), native_cb->get_bool(cb, true, flag));
    ASSERT_EQ(cb->get_bool(false, flag), native_cb->get_bool(cb, false, flag));

    for (auto list : {std::vector<int32_t>{1, 2}, std::vector<int32_t>{0, 1}}) {
        ASSERT_EQ(cb->get_list(list, flag), native_cb->get_list(cb, list, flag));
    }

    for (auto str : {"Hello", "World"}) {
        ASSERT_EQ(cb->get_string(str, flag), native_cb->get_string(cb, str, flag));
    }

    for (auto opt : {std::optional<std::string>("Hello"), std::optional<std::string>(std::nullopt)}) {
        ASSERT_EQ(cb->get_option(opt, flag), native_cb->get_option(cb, opt, flag));
    }

    ASSERT_EQ("TestString", native_cb->get_string_optional_callback(cb, "TestString", false));
    ASSERT_EQ(native_cb->get_string_optional_callback(nullptr, "TestString", false), std::nullopt);

    for (auto bytes : {std::vector<uint8_t>{1, 2}, std::vector<uint8_t>{0, 1}}) {
        ASSERT_EQ(cb->get_bytes(bytes, flag), native_cb->get_bytes(cb, bytes, flag));
    }

    for (auto enum_val : {custom_fixture_callbacks::Enumeration::A, custom_fixture_callbacks::Enumeration::B, custom_fixture_callbacks::Enumeration::C}) {
        for (auto variant : {0, 1, 2}) {
            ASSERT_EQ(cb->get_enum(enum_val, variant, flag), native_cb->get_enum(cb, enum_val, variant, flag));
        }
    }

    ASSERT_EQ(native_cb->get_enum(cb, custom_fixture_callbacks::Enumeration::B, 0, false), custom_fixture_callbacks::Enumeration::B);
    ASSERT_EQ(native_cb->get_enum(cb, custom_fixture_callbacks::Enumeration::B, 10, true), custom_fixture_callbacks::Enumeration::UNKNOWN);

    auto stringifier = std::make_shared<CppStringifier>();
    auto native_stringifier = custom_fixture_callbacks::NativeStringifier::init(stringifier);

    for (auto num : {1, 2}) {
        ASSERT_EQ(stringifier->from_simple_type(num), native_stringifier->from_simple_type(num));
    }

    for (auto num : { std::optional<std::vector<std::optional<double>>>(), std::optional<std::vector<std::optional<double>>>(std::vector<std::optional<double>>{std::nullopt, 1.0, 2.0}) }) {
        ASSERT_EQ(stringifier->from_complex_type(num), native_stringifier->from_complex_type(num));
    }

    return 0;
}
