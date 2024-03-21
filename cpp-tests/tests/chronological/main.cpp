#include <test_common.hpp>

#include <chronological.hpp>
#include <ctime>
#include <iomanip>
#include <thread>
#include <sstream>

using namespace std::chrono_literals;

std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> epoch_second(int seconds, int nanoseconds) {
    return std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(std::chrono::seconds(seconds)) + std::chrono::nanoseconds(nanoseconds);
}


std::chrono::duration<int64_t, std::nano> time_span_seconds(int seconds, int nanoseconds) {
    return std::chrono::seconds(seconds) + std::chrono::nanoseconds(nanoseconds);
}

std::chrono::time_point<std::chrono::system_clock> time_from_string(const std::string& time) {
    std::tm tm = {};
    auto ss = std::stringstream(time);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

#ifdef _WIN32
    return std::chrono::system_clock::from_time_t(_mkgmtime(&tm));
#else
    return std::chrono::system_clock::from_time_t(timegm(&tm));
#endif
}

void test_string_timestamps() {
    // Test post-epoch timestamps
    {
        auto time_str = "1970-12-12T00:00:00.000000000Z";
        auto time = time_from_string(time_str);

        ASSERT_EQ(time_str, chronological::to_string_timestamp(time));
    }

    {
        // get_time doesn't support nanoseconds, so we have to add them manually
        auto time_str = "1970-12-31T23:59:58.999999900Z";
        auto time = time_from_string(time_str) + 999999900ns;

        ASSERT_EQ(time_str, chronological::to_string_timestamp(time));
    }

    {
        // get_time doesn't support nanoseconds, so we have to add them manually
        auto time = time_from_string("2024-11-05T00:06:01.283000200Z") + 283000200ns;
        auto time2 = time_from_string("2024-11-05T00:06:00.283000100Z") + 283000100ns;

        ASSERT_EQ(
            time,
            chronological::add(time2, time_span_seconds(1, 100))
        );
    }

#ifdef _WIN32
    // _mkgmtime doesn't support dates before 1970, so we skip these tests
    return;
#endif

    // Test pre-epoch timestamps
    {
        auto time_str = "1969-12-12T00:00:00.000000000Z";
        auto time = time_from_string(time_str);

        ASSERT_EQ(time_str, chronological::to_string_timestamp(time));
    }

    {
        // get_time doesn't support nanoseconds, so we have to add them manually
        auto time_str = "1969-12-31T23:59:58.999999900Z";
        auto time = time_from_string(time_str) + 999999900ns;

        ASSERT_EQ(time_str, chronological::to_string_timestamp(time));
    }

    {
        // get_time doesn't support nanoseconds, so we have to add them manually
        auto time = time_from_string("1955-11-05T00:06:01.283000200Z") + 283000200ns;
        auto time2 = time_from_string("1955-11-05T00:06:00.283000100Z") + 283000100ns;

        ASSERT_EQ(
            time,
            chronological::add(time2, time_span_seconds(1, 100))
        );
    }
}

int main() {
    ASSERT_EQ(
        epoch_second(101, 200),
        chronological::add(epoch_second(100, 100), time_span_seconds(1, 100))
    );

    ASSERT_EQ(
        time_span_seconds(1, 100),
        chronological::diff(epoch_second(101, 200), epoch_second(100, 100))
    );

    EXPECT_EXCEPTION(
        chronological::diff(epoch_second(100, 0), epoch_second(101, 0)),
        chronological::ChronologicalError
    );


    // When testing the min and max values, we need to round them to the nearest 100 ns, as on Windows one tick is 100 ns
    auto min = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>::min().time_since_epoch().count() / 100 * 100;
    auto min_timepoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(std::chrono::nanoseconds(min));
    ASSERT_EQ(
        min_timepoint,
        chronological::return_timestamp(min_timepoint)
    );

    auto max = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>::max().time_since_epoch().count() / 100 * 100;
    auto max_timepoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(std::chrono::nanoseconds(max));
    ASSERT_EQ(
        max_timepoint,
        chronological::return_timestamp(max_timepoint)
    );

    ASSERT_EQ(
        std::chrono::nanoseconds::max(),
        chronological::return_duration(std::chrono::nanoseconds::max())
    );

    test_string_timestamps();

    auto before = std::chrono::system_clock::now();
    std::this_thread::sleep_for(1s);
    auto after = chronological::now();
    std::this_thread::sleep_for(1s);
    auto after2 = std::chrono::system_clock::now();
    ASSERT_EQ(-1, std::chrono::duration_cast<std::chrono::seconds>(before - after).count());
    ASSERT_EQ(1, std::chrono::duration_cast<std::chrono::seconds>(after2 - after).count());

    ASSERT_TRUE(chronological::optional(std::chrono::time_point<std::chrono::system_clock>::max(), time_span_seconds(0, 0)));
    ASSERT_FALSE(chronological::optional(std::nullopt, time_span_seconds(0, 0)));
    ASSERT_FALSE(chronological::optional(std::chrono::time_point<std::chrono::system_clock>::min(), std::nullopt));

    return 0;
}
