#include "lib_chronological.hpp"

#include <ctime>
#include <sstream>
#include <iomanip>

chronological::timestamp chronological::return_timestamp(chronological::timestamp a) {
    return a;
}

chronological::duration chronological::return_duration(chronological::duration a) {
    return a;
}

std::string chronological::to_string_timestamp(chronological::timestamp a) {
    using time_point = std::chrono::system_clock::time_point;
    std::time_t time = std::chrono::system_clock::to_time_t(time_point {std::chrono::duration_cast<time_point::duration>(a.time_since_epoch())});
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(a.time_since_epoch()).count() % 1000000000;
    if (ns < 0) {
        ns += 1000000000;
        time -= 1;
    }

    std::tm tm = *std::gmtime(&time);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    ss<< "." << std::setfill('0') << std::setw(9) << ns;
    ss << "Z";

    return ss.str();
}

chronological::timestamp chronological::get_pre_epoch_timestamp() {
    return std::chrono::time_point<std::chrono::system_clock>::min();
}

chronological::timestamp chronological::add(chronological::timestamp a, chronological::duration b) {
    return std::chrono::time_point_cast<std::chrono::system_clock::duration>(a + b);
}

chronological::duration chronological::diff(chronological::timestamp a, chronological::timestamp b) {
    if (a < b) {
        throw chronological_error::TimeDiffError();
    }
    return a - b;
}

chronological::timestamp chronological::now() {
    return std::chrono::system_clock::now();
}

bool chronological::equal(chronological::timestamp a, chronological::timestamp b) {
    return a == b;
}

bool chronological::optional(std::optional<chronological::timestamp> a, std::optional<chronological::duration> b) {
    return a.has_value() && b.has_value();
}

uint64_t chronological::get_seconds_before_unix_epoch(chronological::timestamp a) {
    return std::chrono::duration_cast<std::chrono::seconds>(a.time_since_epoch()).count();
}

chronological::timestamp chronological::set_seconds_before_unix_epoch(uint64_t seconds) {
    return std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(seconds));
}

#include <chronological_cpp_scaffolding.cpp>

