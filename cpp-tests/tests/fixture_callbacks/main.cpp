#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <optional>
#include <stdexcept>
#include <test_common.hpp>

#include <string>
#include <algorithm>
#include <cctype>
#include <fixture_callbacks.hpp>

struct CppGetters : public fixture_callbacks::ForeignGetters {
    bool get_bool(bool v, bool argument_two) override { return v ^ argument_two; }

    std::string get_string(const std::string &v, bool arg2) override {
        if (v.compare("bad-argument") == 0) {
            throw fixture_callbacks::simple_error::BadArgument("bard argument");
        }

        if (v.compare("unexpected-error") == 0) {
            throw std::runtime_error("something failed");
        }

        return arg2 ? "1234567890123" : v;
    }

    std::optional<std::string> get_option(std::optional<std::string> v, bool arg2) override {
        if (v.has_value()) {
            if (v.value().compare("bad-argument") == 0) {
                auto e = fixture_callbacks::complex_error::ReallyBadArgument();
                e.code = 20;
                throw e;
            }

            if (v.value().compare("unexpected-error") == 0) {
                throw std::runtime_error("something failed");
            }
        }

        if (arg2 && v.has_value()) {
            std::transform(v.value().begin(), v.value().end(), v.value().begin(), [](char c) { return ::toupper(c); });
        }

        return v;
    }

    std::vector<int32_t> get_list(const std::vector<int32_t> &v, bool arg2) override { return arg2 ? v : std::vector<int32_t>(); }

    void get_nothing(const std::string &v) override {
        if (v.compare("bad-argument") == 0) {
            throw fixture_callbacks::simple_error::BadArgument("bad argument");
        }

        if (v.compare("unexpected-error") == 0) {
            throw std::runtime_error("something failed");
        }
    }
};

struct CppStringifier : public fixture_callbacks::StoredForeignStringifier {
    std::string from_simple_type(int32_t value) override { return "C++: " + std::to_string(value); }

    // This is not getting directly tested in the UniFFI tests, it's used to check for compilation errors
    std::string from_complex_type(std::optional<std::vector<std::optional<double>>> values) override { return "C++"; }
};

void test_callback_roundtrip() {
    auto callback      = std::make_shared<CppGetters>();
    auto rust_callback = fixture_callbacks::RustGetters::init();

    bool flag = true;

    ASSERT_EQ(callback->get_string("test", flag), rust_callback->get_string(callback, "test", flag));
    ASSERT_EQ(callback->get_bool(true, flag), rust_callback->get_bool(callback, true, flag));
    ASSERT_EQ(callback->get_bool(false, flag), rust_callback->get_bool(callback, false, flag));

    for (auto list : {std::vector<int32_t>{1, 2}, std::vector<int32_t>{0, 1}}) {
        ASSERT_EQ(callback->get_list(list, flag), rust_callback->get_list(callback, list, flag));
    }

    for (auto str : {"Hello", "World"}) {
        ASSERT_EQ(callback->get_string(str, flag), rust_callback->get_string(callback, str, flag));
    }

    for (auto opt : {std::optional<std::string>("Hello"), std::optional<std::string>(std::nullopt)}) {
        ASSERT_EQ(callback->get_option(opt, flag), rust_callback->get_option(callback, opt, flag));
    }

    ASSERT_EQ("TestString", rust_callback->get_string_optional_callback(callback, "TestString", false));
    ASSERT_EQ(rust_callback->get_string_optional_callback(nullptr, "TestString", false), std::nullopt);
}

void test_callback_roundtrip_errors() {
    auto callback      = std::make_shared<CppGetters>();
    auto rust_callback = fixture_callbacks::RustGetters::init();

    bool flag = true;

    EXPECT_EXCEPTION(rust_callback->get_string_optional_callback(callback, "bad-argument", flag), fixture_callbacks::simple_error::BadArgument);
    EXPECT_EXCEPTION(rust_callback->get_string(callback, "unexpected-error", flag), fixture_callbacks::simple_error::UnexpectedError);

    try {
        rust_callback->get_option(callback, "bad-argument", flag);
    } catch (fixture_callbacks::complex_error::ReallyBadArgument &e) {
        ASSERT_EQ(e.code, 20);
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        rust_callback->get_option(callback, "unexpected-error", flag);
    } catch (fixture_callbacks::complex_error::UnexpectedErrorWithReason &e) {
        ASSERT_EQ(e.reason.compare("something failed"), 0);
    } catch (...) {
        ASSERT_TRUE(false);
    }
}

void test_callback_stored_in_object() {
    auto stringifier = std::make_shared<CppStringifier>();
    auto rust_stringifier = fixture_callbacks::RustStringifier::init(stringifier);

    for (auto num : {1, 2}) {
        ASSERT_EQ(stringifier->from_simple_type(num), rust_stringifier->from_simple_type(num));
    }
}

void test_void_callback_exceptions() {
    auto callback      = std::make_shared<CppGetters>();
    auto rust_callback = fixture_callbacks::RustGetters::init();

    try {
        rust_callback->get_nothing(callback, "foo");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    EXPECT_EXCEPTION(rust_callback->get_nothing(callback, "bad-argument"), fixture_callbacks::simple_error::BadArgument);
    EXPECT_EXCEPTION(rust_callback->get_nothing(callback, "unexpected-error"), fixture_callbacks::simple_error::UnexpectedError);
}

int main() {
    test_callback_roundtrip();
    test_callback_roundtrip_errors();
    test_callback_stored_in_object();
    test_void_callback_exceptions();

    return 0;
}
