#include "test_common.hpp"

#include <thread>
#include <coverall.hpp>

void test_some_dict() {
    auto dict = coverall::create_some_dict();

    ASSERT_EQ("text", dict.text);
    ASSERT_EQ("maybe_text", dict.maybe_text);
    ASSERT_EQ("some_bytes", std::string(dict.some_bytes.begin(), dict.some_bytes.end()));
    ASSERT_EQ("maybe_some_bytes", std::string(dict.maybe_some_bytes->begin(), dict.maybe_some_bytes->end()));
    ASSERT_EQ(true, dict.a_bool);
    ASSERT_EQ(false, dict.maybe_a_bool);
    ASSERT_EQ(1, dict.unsigned8);
    ASSERT_EQ(2, dict.maybe_unsigned8);
    ASSERT_EQ(3, dict.unsigned16);
    ASSERT_EQ(4, dict.maybe_unsigned16);
    ASSERT_EQ(std::numeric_limits<uint64_t>::max(), dict.unsigned64);
    ASSERT_EQ(std::numeric_limits<uint64_t>::min(), dict.maybe_unsigned64);
    ASSERT_EQ(8, dict.signed8);
    ASSERT_EQ(0, dict.maybe_signed8);
    ASSERT_EQ(std::numeric_limits<int64_t>::max(), dict.signed64);
    ASSERT_EQ(0, dict.maybe_signed64);
    ASSERT_EQ(1.2345f, dict.float32);
    ASSERT_EQ(22.0f / 7.0f, dict.maybe_float32);
    ASSERT_EQ(0.0, dict.float64);
    ASSERT_EQ(1.0, dict.maybe_float64);
    ASSERT_EQ("some_dict", dict.coveralls->get_name());
    ASSERT_EQ("node-2", dict.test_trait->name());
}

void test_none_dict() {
    auto dict = coverall::create_none_dict();
    ASSERT_EQ("text", dict.text);
    ASSERT_EQ(std::nullopt, dict.maybe_text);
    ASSERT_EQ("some_bytes", std::string(dict.some_bytes.begin(), dict.some_bytes.end()));
    ASSERT_EQ(std::nullopt, dict.maybe_some_bytes);
    ASSERT_EQ(true, dict.a_bool);
    ASSERT_EQ(std::nullopt, dict.maybe_a_bool);
    ASSERT_EQ(1, dict.unsigned8);
    ASSERT_EQ(std::nullopt, dict.maybe_unsigned8);
    ASSERT_EQ(3, dict.unsigned16);
    ASSERT_EQ(std::nullopt, dict.maybe_unsigned16);
    ASSERT_EQ(18446744073709551615, dict.unsigned64);
    ASSERT_EQ(std::nullopt, dict.maybe_unsigned64);
    ASSERT_EQ(8, dict.signed8);
    ASSERT_EQ(std::nullopt, dict.maybe_signed8);
    ASSERT_EQ(9223372036854775807, dict.signed64);
    ASSERT_EQ(std::nullopt, dict.maybe_signed64);
    ASSERT_EQ(1.2345f, dict.float32);
    ASSERT_EQ(std::nullopt, dict.maybe_float32);
    ASSERT_EQ(0.0, dict.float64);
    ASSERT_EQ(std::nullopt, dict.maybe_float64);
    ASSERT_EQ(nullptr, dict.coveralls);
    ASSERT_EQ(nullptr, dict.test_trait);
}

void test_constructors() {
    {
        ASSERT_EQ(0ul, coverall::get_num_alive());

        auto coveralls = coverall::Coveralls::init("c1");
        ASSERT_EQ(1ul, coverall::get_num_alive());
        ASSERT_EQ("c1", coveralls->get_name());

        auto coveralls2 = coverall::Coveralls::fallible_new("c2", false);
        ASSERT_EQ(2ul, coverall::get_num_alive());
        ASSERT_EQ("c2", coveralls2->get_name());

        EXPECT_EXCEPTION(coverall::Coveralls::fallible_new("", true), coverall::coverall_error::TooManyHoles);

        try {
            coverall::Coveralls::panicking_new("expected panic: woe is me");
            ASSERT_TRUE(false);
        } catch (std::runtime_error &e) {
            ASSERT_EQ(std::string("expected panic: woe is me"), e.what());
        } catch (...) {
            ASSERT_TRUE(false);
        }
    }

    ASSERT_EQ(0ul, coverall::get_num_alive());
}

void test_arcs() {
    {
        auto coveralls = coverall::Coveralls::init("test_arcs");

        ASSERT_EQ(1ul, coverall::get_num_alive());
        ASSERT_EQ(2ul, coveralls->strong_count());
        ASSERT_EQ(nullptr, coveralls->get_other());

        coveralls->take_other(coveralls);
        ASSERT_EQ(3ul, coveralls->strong_count());
        ASSERT_EQ(1ul, coverall::get_num_alive());

        {
            auto other = coveralls->get_other();
            ASSERT_EQ("test_arcs", other->get_name());
        }

        EXPECT_EXCEPTION(coveralls->take_other_fallible(), coverall::coverall_error::TooManyHoles);
        EXPECT_EXCEPTION(coveralls->take_other_panic("take_other_panic"), std::runtime_error);
        EXPECT_EXCEPTION(coveralls->fallible_panic("fallible_panic"), std::runtime_error);

        coveralls->take_other(nullptr);
        ASSERT_EQ(2ul, coveralls->strong_count());
    }

    ASSERT_EQ(0ul, coverall::get_num_alive());
}

void test_object_returning() {
    {
        auto coveralls = coverall::Coveralls::init("test_object_returning");

        ASSERT_EQ(1ul, coverall::get_num_alive());
        ASSERT_EQ(2ul, coveralls->strong_count());
        {
            auto other = coveralls->clone_me();
            ASSERT_EQ(coveralls->get_name(), other->get_name());
            ASSERT_EQ(2ul, coverall::get_num_alive());
            ASSERT_EQ(2ul, other->strong_count());

            coveralls->take_other(other);
            ASSERT_EQ(2ul, coverall::get_num_alive());
            ASSERT_EQ(2ul, coveralls->strong_count());
            ASSERT_EQ(3ul, other->strong_count());
        }

        ASSERT_EQ(2ul, coverall::get_num_alive());
    }

    ASSERT_EQ(0ul, coverall::get_num_alive());
}

void test_simple_errors() {
    auto coveralls = coverall::Coveralls::init("test_simple_errors");

    try {
        coveralls->maybe_throw(true);
        ASSERT_TRUE(false);
    } catch (coverall::coverall_error::TooManyHoles &e) {
        ASSERT_EQ(std::string("The coverall has too many holes"), e.what());
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        coverall::throw_macro_error();
        ASSERT_TRUE(false);
    } catch (coverall::coverall_macro_error::TooManyMacros &e) {
        ASSERT_EQ(std::string("The coverall has too many macros"), e.what());
    } catch (...) {
        ASSERT_TRUE(false);
    }

    EXPECT_EXCEPTION(coveralls->maybe_throw_into(true), coverall::coverall_error::TooManyHoles);

    try {
        coveralls->panic("expected panic: oh no");
        ASSERT_TRUE(false);
    } catch (std::runtime_error &e) {
        ASSERT_EQ(std::string("expected panic: oh no"), e.what());
    } catch (...) {
        ASSERT_TRUE(false);
    }
}

void test_flat_errors() {
     try {
        coverall::throw_flat_error();
        ASSERT_TRUE(false);
    } catch (coverall::coverall_flat_error::TooManyVariants &e) {
        ASSERT_EQ(std::string("Too many variants: 99"), e.what());
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        coverall::throw_flat_macro_error();
        ASSERT_TRUE(false);
    } catch (coverall::coverall_flat_macro_error::TooManyVariants &e) {
        ASSERT_EQ(std::string("Too many variants: 88"), e.what());
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        coverall::throw_rich_error_no_variant_data();
        ASSERT_TRUE(false);
    } catch (coverall::coverall_rich_error_no_variant_data::TooManyPlainVariants &e) {
        ASSERT_EQ(std::string(""), e.what());
    } catch (...) {
        ASSERT_TRUE(false);
    }
}

void test_complex_errors() {
    auto coveralls = coverall::Coveralls::init("test_complex_errors");

    ASSERT_TRUE(coveralls->maybe_throw_complex(0));

    try {
        coveralls->maybe_throw_complex(1);
        ASSERT_TRUE(false);
    } catch (const coverall::complex_error::OsError& e) {
        ASSERT_EQ(10, e.code);
        ASSERT_EQ(20, e.extended_code);
    }

    try {
        coverall::throw_complex_macro_error();
        ASSERT_TRUE(false);
    } catch (const coverall::complex_macro_error::OsError& e) {
        ASSERT_EQ(1, e.code);
        ASSERT_EQ(2, e.extended_code);
    }

    try {
        coveralls->maybe_throw_complex(2);
        ASSERT_TRUE(false);
    } catch (const coverall::complex_error::PermissionDenied& e) {
        ASSERT_EQ("Forbidden", e.reason);
    }

    EXPECT_EXCEPTION(coveralls->maybe_throw_complex(3), coverall::complex_error::UnknownError);
    EXPECT_EXCEPTION(coveralls->maybe_throw_complex(4), std::runtime_error);
}

void test_error_values() {
    EXPECT_EXCEPTION(coverall::throw_root_error(), coverall::root_error::Complex);

    auto e = coverall::get_root_error();
    auto subtype = dynamic_cast<coverall::root_error::Other*>(e.get());
    ASSERT_TRUE(subtype != nullptr);
    ASSERT_EQ(coverall::OtherError::kUnexpected, subtype->error);

    ASSERT_TRUE(dynamic_cast<coverall::complex_error::PermissionDenied*>(coverall::get_complex_error(nullptr).get()) != nullptr);
    ASSERT_TRUE(coverall::get_error_dict(std::nullopt).complex_error == nullptr);
}

void test_enums() {
    auto e = coverall::get_simple_flat_macro_enum(0);
    ASSERT_TRUE(std::holds_alternative<coverall::SimpleFlatMacroEnum::kFirst>(e.get_variant()));

}

void test_self_by_arc() {
    auto coveralls = coverall::Coveralls::init("test_self_by_arc");
    ASSERT_EQ(2ul, coveralls->strong_count());
}

void test_throwing_constructor() {
    EXPECT_EXCEPTION(coverall::FalliblePatch::init(), coverall::coverall_error::TooManyHoles);
    EXPECT_EXCEPTION(coverall::FalliblePatch::secondary(), coverall::coverall_error::TooManyHoles);
}

void test_interface_in_dicts() {
    auto coveralls = coverall::Coveralls::init("test_interface_in_dicts");

    coveralls->add_patch(coverall::Patch::init(coverall::Color::kRed));
    coveralls->add_repair(coverall::Repair {.when = std::chrono::system_clock::now(), .patch = coverall::Patch::init(coverall::Color::kGreen)});
    ASSERT_EQ(2, coveralls->get_repairs().size());
}

void test_multithread_calls() {
    const int WAIT_TIME = 20;

    auto counter = coverall::ThreadsafeCounter::init();

    auto thread = std::thread([&counter]() {
        counter->busy_wait(WAIT_TIME);
    });

    auto count = 0;
    auto count_thread = std::thread([&counter, &count]() {
        for(auto i = 0; i < WAIT_TIME; i++) {
            count = counter->increment_if_busy();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    thread.join();
    count_thread.join();
    ASSERT_TRUE(count > 0);
}

void test_bytes() {
    auto coveralls = coverall::Coveralls::init("test_bytes");

    ASSERT_EQ(std::vector<uint8_t>({1, 2, 3}), coveralls->reverse(std::vector<uint8_t>({3, 2, 1})));
}

void test_dict_with_defaults() {
    auto d = coverall::DictWithDefaults();
    ASSERT_EQ("default-value", d.name);
    ASSERT_EQ(std::nullopt, d.category);
    ASSERT_EQ(31, d.integer);
}

void test_dict_with_non_string_keys() {
    auto coveralls = coverall::Coveralls::init("test_dict");

    auto dict1 = coveralls->get_dict("answer", 42);
    ASSERT_EQ(42, dict1.at("answer"));

    auto dict2 = coveralls->get_dict2("answer", 42);
    ASSERT_EQ(42, dict2.at("answer"));

    auto dict3 = coveralls->get_dict3(31, 42);
    ASSERT_EQ(42, dict3.at(31));
}

void test_return_only_dict() {
    auto d = coverall::ReturnOnlyDict{
        .e = std::make_shared<coverall::coverall_flat_error::TooManyVariants>()
    };
    EXPECT_EXCEPTION(coverall::try_input_return_only_dict(d), std::runtime_error);
}


struct CppGetters : public coverall::Getters {
    bool get_bool(bool v, bool arg2) override {
        return v ^ arg2;
    }

    std::string get_string(const std::string &v, bool arg2) override {
        if (v == "too-many-holes") {
            throw coverall::coverall_error::TooManyHoles();
        }
        else if (v == "unexpected-error") {
            throw std::runtime_error("unexpected error");
        }
        else if (arg2 == true) {
            std::string val = v;
            std::transform(val.begin(), val.end(), val.begin(), [](char c) { return ::toupper(c); });

            return val;
        } else {
            return v;
        }
    }

    std::optional<std::string> get_option(const std::string &v, bool arg2) override {
        if (v == "os-error") {
            auto err = coverall::complex_error::OsError();
            err.code = 100;
            err.extended_code = 200;

            throw err;
        }
        else if (v == "unknown-error") {
            throw coverall::complex_error::UnknownError();
        }
        else if (arg2 == true) {
            if (!v.empty()) {
                std::string val = v;
                std::transform(val.begin(), val.end(), val.begin(), [](char c) { return ::toupper(c); });

                return val;
            }
            else {
                return std::nullopt;
            }
        }
        else {
            return v;
        }
    }

    std::vector<int32_t> get_list(const std::vector<int32_t> &v, bool arg2) override {
        if (arg2 == true) {
            return v;
        } else {
            return {};
        }
    }

    void get_nothing(const std::string &v) override {}

    std::shared_ptr<coverall::Coveralls> round_trip_object(const std::shared_ptr<coverall::Coveralls> &coveralls) override {
        return coveralls;
    }
};

struct CppNode : public coverall::NodeTrait {
    std::shared_ptr<coverall::NodeTrait> parent = nullptr;

    std::string name() override {
        return "node-cpp";
    }

    void set_parent(std::shared_ptr<NodeTrait> parent) override {
        this->parent = parent;
    }

    std::shared_ptr<NodeTrait> get_parent() override {
        return this->parent;
    }

    uint64_t strong_count() override {
        return 0;
    }
};

void test_cpp_getters() {
    coverall::test_getters(std::make_shared<CppGetters>());

    auto getters = std::make_shared<CppGetters>();
    ASSERT_EQ(false, getters->get_bool(true, true));
    ASSERT_EQ(true, getters->get_bool(true, false));
    ASSERT_EQ(true, getters->get_bool(false, true));
    ASSERT_EQ(false, getters->get_bool(false, false));

    ASSERT_EQ("hello", getters->get_string("hello", false));
    ASSERT_EQ("HELLO", getters->get_string("hello", true));

    ASSERT_EQ("HELLO", getters->get_option("hello", true));
    ASSERT_EQ("hello", getters->get_option("hello", false));
    ASSERT_EQ(std::nullopt, getters->get_option("", true));

    auto expected_list = std::vector<int32_t> {1, 2, 3};
    ASSERT_EQ(expected_list, getters->get_list({1, 2, 3}, true));
    ASSERT_EQ(std::vector<int32_t>(), getters->get_list({1, 2, 3}, false));

    EXPECT_EXCEPTION(getters->get_string("too-many-holes", true), coverall::coverall_error::TooManyHoles);
    try {
        getters->get_option("os-error", true);
        ASSERT_TRUE(false);
    } catch (const coverall::complex_error::OsError& e) {
        ASSERT_EQ(100, e.code);
        ASSERT_EQ(200, e.extended_code);
    }
    EXPECT_EXCEPTION(getters->get_option("unknown-error", true), coverall::complex_error::UnknownError);
    EXPECT_EXCEPTION(getters->get_string("unexpected-error", true), std::runtime_error);
}

void test_path() {
    auto traits = coverall::get_traits();

    ASSERT_EQ("node-1", traits[0]->name());
    ASSERT_EQ(2, traits[0]->strong_count());

    ASSERT_EQ("node-2", traits[1]->name());
    ASSERT_EQ(2, traits[1]->strong_count());

    traits[0]->set_parent(traits[1]);
    ASSERT_EQ(2, traits[1]->strong_count());

    ASSERT_EQ(std::vector<std::string> {"node-2"} , coverall::ancestor_names(traits[0]));
    ASSERT_EQ(std::vector<std::string> {} , coverall::ancestor_names(traits[1]));

    auto cpp_node = std::make_shared<CppNode>();
    traits[1]->set_parent(cpp_node);

    {
        auto list = std::vector<std::string>  {"node-2", "node-cpp"};
        ASSERT_EQ(list, coverall::ancestor_names(traits[0]));
    }

    {
        auto list = std::vector<std::string>  {"node-cpp"};
        ASSERT_EQ(list, coverall::ancestor_names(traits[1]));
    }

    {
        auto list = std::vector<std::string> {};
        ASSERT_EQ(list, coverall::ancestor_names(cpp_node));
    }

    traits[1]->set_parent(nullptr);
    cpp_node->set_parent(traits[0]);

    {
        auto list = std::vector<std::string>  {"node-1", "node-2"};
        ASSERT_EQ(list, coverall::ancestor_names(cpp_node));
    }

    {
        auto list = std::vector<std::string>  {"node-2"};
        ASSERT_EQ(list, coverall::ancestor_names(traits[0]));
    }

    {
        auto list = std::vector<std::string> {};
        ASSERT_EQ(list, coverall::ancestor_names(traits[1]));
    }

    cpp_node->set_parent(nullptr);
    traits[0]->set_parent(nullptr);
}

void test_round_tripping() {
    auto rust_getters = coverall::make_rust_getters();
    auto coveralls = coverall::Coveralls::init("test_round_tripping");

    coverall::test_round_trip_through_rust(rust_getters);
    coverall::test_round_trip_through_foreign(std::make_shared<CppGetters>());
}

void test_rust_only_traits() {
    auto traits = coverall::get_string_util_traits();
    ASSERT_EQ("cowboy", traits[0]->concat("cow", "boy"));
    ASSERT_EQ("cowboy", traits[1]->concat("cow", "boy"));
}

void test_html_error() {
    EXPECT_EXCEPTION(coverall::validate_html("test"), coverall::HtmlError);
}

int main() {
    test_some_dict();
    test_constructors();
    test_arcs();
    test_self_by_arc();
    test_throwing_constructor();
    test_object_returning();
    test_simple_errors();
    test_flat_errors();
    test_error_values();
    test_enums();
    test_complex_errors();
    test_interface_in_dicts();
    test_multithread_calls();
    test_bytes();
    test_dict_with_defaults();
    test_dict_with_non_string_keys();
    test_return_only_dict();
    test_cpp_getters();
    test_path();
    test_round_tripping();
    test_rust_only_traits();
    test_html_error();

    return 0;
}
