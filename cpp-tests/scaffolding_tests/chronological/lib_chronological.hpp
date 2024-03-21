#include <chrono>
#include <optional>
#include <string>
#include <stdexcept>

namespace {
    namespace chronological {
        typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> timestamp;
        typedef std::chrono::duration<int64_t, std::nano> duration;

        namespace chronological_error {
            class TimeOverflow: public std::runtime_error {
                public:
                    TimeOverflow(): std::runtime_error("Time overflow") { }
            };

            class TimeDiffError: public std::runtime_error {
                public:
                    TimeDiffError(): std::runtime_error("Time Diff error") { }
            };
        };

        timestamp return_timestamp(timestamp a);
        duration return_duration(duration a);
        std::string to_string_timestamp(timestamp a);
        timestamp get_pre_epoch_timestamp();
        timestamp add(timestamp a, duration b);
        duration diff(timestamp a, timestamp b);
        timestamp now();
        bool equal(timestamp a, timestamp b);
        bool optional(std::optional<timestamp> a, std::optional<duration> b);
        uint64_t get_seconds_before_unix_epoch(timestamp a);
        timestamp set_seconds_before_unix_epoch(uint64_t seconds);
    }
}
