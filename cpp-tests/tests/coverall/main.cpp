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

    EXPECT_EXCEPTION(coveralls->maybe_throw(true), coverall::coverall_error::TooManyHoles);
    EXPECT_EXCEPTION(coveralls->maybe_throw_into(true), coverall::coverall_error::TooManyHoles);
    EXPECT_EXCEPTION(coveralls->panic("coveralls->panic()"), std::runtime_error);
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
        coveralls->maybe_throw_complex(2);
        ASSERT_TRUE(false);
    } catch (const coverall::complex_error::PermissionDenied& e) {
        ASSERT_EQ("Forbidden", e.reason);

        EXPECT_EXCEPTION(coveralls->maybe_throw_complex(3), coverall::complex_error::UnknownError);
        EXPECT_EXCEPTION(coveralls->maybe_throw_complex(4), std::runtime_error);
    }
}

void test_interface_in_dicts() {
    auto coveralls = coverall::Coveralls::init("test_interface_in_dicts");

    coveralls->add_patch(coverall::Patch::init(coverall::Color::RED));
    coveralls->add_repair(coverall::Repair {.when = std::chrono::system_clock::now(), .patch = coverall::Patch::init(coverall::Color::GREEN)});
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

int main() {
    test_some_dict();
    test_arcs();
    test_object_returning();
    test_simple_errors();
    test_complex_errors();
    test_interface_in_dicts();
    test_multithread_calls();
    test_bytes();

    return 0;
}