#include <test_common.hpp>

#include <custom_types.hpp>

int main() {
    auto demo = custom_types::get_custom_types_demo(std::nullopt);
    ASSERT_EQ("http://example.com/", demo.url);
    ASSERT_EQ(123, demo.handle);
    ASSERT_EQ(456000, demo.time_interval_ms);
    ASSERT_EQ(456.0, demo.time_interval_sec_dbl);
    ASSERT_EQ(777.0f, demo.time_interval_sec_flt);

    auto demo2 = custom_types::CustomTypesDemo {
        "http://example.com/2",
        456,
        532000,
        532.0,
        333.0f
    };
    auto ret = custom_types::get_custom_types_demo(demo2);
    ASSERT_EQ(demo2.url, ret.url);
    ASSERT_EQ(demo2.handle, ret.handle);
    ASSERT_EQ(demo2.time_interval_ms, ret.time_interval_ms);
    ASSERT_EQ(demo2.time_interval_sec_dbl, ret.time_interval_sec_dbl);
    ASSERT_EQ(demo2.time_interval_sec_flt, ret.time_interval_sec_flt);

    return 0;
}