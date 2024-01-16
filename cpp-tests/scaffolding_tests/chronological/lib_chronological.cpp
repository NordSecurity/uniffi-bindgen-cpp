#include "lib_chronological.hpp"

#include <ctime>
#include <sstream>
#include <iomanip>

lib_chronological::timestamp lib_chronological::return_timestamp(lib_chronological::timestamp a) {
    return a;
}

lib_chronological::duration lib_chronological::return_duration(lib_chronological::duration a) {
    return a;
}

std::string lib_chronological::to_string_timestamp(lib_chronological::timestamp a) {
    std::time_t time = std::chrono::system_clock::to_time_t(a);
    std::tm tm = *std::gmtime(&time);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.000000000Z");
    return ss.str();
}

lib_chronological::timestamp lib_chronological::get_pre_epoch_timestamp() {
    return std::chrono::time_point<std::chrono::system_clock>::min();
}

lib_chronological::timestamp lib_chronological::add(lib_chronological::timestamp a, lib_chronological::duration b) {
    return a + b;
}

lib_chronological::duration lib_chronological::diff(lib_chronological::timestamp a, lib_chronological::timestamp b) {
    return a - b;
}

lib_chronological::timestamp lib_chronological::now() {
    return std::chrono::system_clock::now();
}

bool lib_chronological::equal(lib_chronological::timestamp a, lib_chronological::timestamp b) {
    return a == b;
}

bool lib_chronological::optional(std::optional<lib_chronological::timestamp> a, std::optional<lib_chronological::duration> b) {
    return a.has_value() && b.has_value();
}

uint64_t lib_chronological::get_seconds_before_unix_epoch(lib_chronological::timestamp a) {
    return std::chrono::duration_cast<std::chrono::seconds>(a.time_since_epoch()).count();
}

lib_chronological::timestamp lib_chronological::set_seconds_before_unix_epoch(uint64_t seconds) {
    return std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(seconds));
}

