#include "lib_coverall.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

std::atomic_uint64_t NUM_ALIVE = 0;

coverall::Coveralls::Coveralls(std::string name, bool should_panic) {
    if (!should_panic) {
        NUM_ALIVE += 1;
        this->name = name;
    } else {
        throw coverall_error::TooManyHoles();
    }
}

coverall::Coveralls::Coveralls(std::string name) {
    NUM_ALIVE += 1;
    this->name = name;
}

coverall::Coveralls::~Coveralls() {
    NUM_ALIVE -= 1;
}

bool coverall::IFirst::compare(std::shared_ptr<ISecond> other) {
    return false;
}

bool coverall::ISecond::compare(std::shared_ptr<IFirst> other) {
    return false;
}

void coverall::ThreadsafeCounter::busy_wait(int32_t ms) {
    this->is_busy = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    this->is_busy = false;
}

int32_t coverall::ThreadsafeCounter::increment_if_busy() {
    if (this->is_busy) {
        return this->count += 1;
    } else {
        return this->count;
    }
}

coverall::Color coverall::Patch::get_color() {
    return this->color;
}

std::string coverall::Coveralls::get_name() {
    return this->name;
}

bool coverall::Coveralls::maybe_throw(bool should_throw) {
    if (should_throw) {
        throw coverall_error::TooManyHoles();
    } else {
        return true;
    }
}

// Note: This should test Rust's Into, we don't really have this here
bool coverall::Coveralls::maybe_throw_into(bool should_throw) {
    if (should_throw) {
        throw coverall_error::TooManyHoles();
    } else {
            return true;
    }
}

bool coverall::Coveralls::maybe_throw_complex(uint8_t input) {
    switch(input) {
    case 0:
        return true;
    case 1: {
        auto err = complex_error::OsError();
        err.code = 10;
        err.extended_code = 20;

        throw err;
    }
    case 2: {
        auto err = complex_error::PermissionDenied();
        err.reason = "Forbidden";

        throw err;
    }
    case 3:
        throw complex_error::UnknownError();
    default:
        throw std::runtime_error("Invalid input");
    }
}

void coverall::Coveralls::panic(std::string message) {
    throw std::runtime_error("coveralls->panic()");
}

void coverall::Coveralls::fallible_panic(std::string message) {
    throw std::runtime_error(message);
}

uint64_t coverall::Coveralls::strong_count(const std::shared_ptr<Coveralls> &self) {
    return self.use_count();
}

void coverall::Coveralls::take_other(const std::shared_ptr<Coveralls> &other) {
    std::lock_guard<std::mutex> lock(this->other_mutex);
    this->other = other;
}

std::shared_ptr<coverall::Coveralls> coverall::Coveralls::get_other() {
    std::lock_guard<std::mutex> lock(this->other_mutex);
    return this->other;
}

void coverall::Coveralls::take_other_fallible(const std::shared_ptr<Coveralls> &self) {
    throw coverall::coverall_error::TooManyHoles();
}

void coverall::Coveralls::take_other_panic(const std::shared_ptr<Coveralls> &self, std::string message) {
    throw std::runtime_error(message);
}

std::shared_ptr<coverall::Coveralls> coverall::Coveralls::clone_me() {
    std::lock_guard<std::mutex> lock(this->other_mutex);

    auto clone = std::make_shared<Coveralls>(this->name);
    clone->other = this->other;

    return clone;
}

std::string coverall::Coveralls::get_status(std::string status) {
    return "status: " + status;
}

std::unordered_map<std::string, uint64_t> coverall::Coveralls::get_dict(std::string key, uint64_t value) {
    std::unordered_map<std::string, uint64_t> map;
    map.insert({ key, value });

    return map;
}

std::unordered_map<std::string, uint64_t> coverall::Coveralls::get_dict2(std::string key, uint64_t value) {
    std::unordered_map<std::string, uint64_t> map;
    map.insert({ key, value });

    return map;
}

std::unordered_map<uint32_t, uint64_t> coverall::Coveralls::get_dict3(uint32_t key, uint64_t value) {
    std::unordered_map<uint32_t, uint64_t> map;
    map.insert({ key, value });

    return map;
}

void coverall::Coveralls::add_patch(std::shared_ptr<Patch> patch) {
    auto repair = Repair {
        .when = std::chrono::high_resolution_clock::now(),
        .patch = patch
    };

    std::lock_guard<std::mutex> lock(this->repairs_mutex);
    this->repairs.push_back(repair);
}

void coverall::Coveralls::add_repair(Repair repair) {
    std::lock_guard<std::mutex> lock(this->repairs_mutex);
    this->repairs.push_back(repair);
}

std::vector<coverall::Repair> coverall::Coveralls::get_repairs() {
    std::lock_guard<std::mutex> lock(this->repairs_mutex);
    return this->repairs;
}

std::vector<uint8_t> coverall::Coveralls::reverse(std::vector<uint8_t> value) {
    return std::vector<uint8_t>(value.rbegin(), value.rend());
}


coverall::SimpleDict coverall::create_some_dict() {
    std::string some_bytes_string = "some_bytes";
    std::string maybe_some_bytes_string = "maybe_some_bytes";

    return {
        .text = "text",
        .maybe_text = "maybe_text",
        .some_bytes = std::vector<uint8_t>(some_bytes_string.begin(), some_bytes_string.end()),
        .maybe_some_bytes = std::vector<uint8_t>(maybe_some_bytes_string.begin(), maybe_some_bytes_string.end()),
        .a_bool = true,
        .maybe_a_bool = false,
        .unsigned8 = 1,
        .maybe_unsigned8 = 2,
        .unsigned16 = 3,
        .maybe_unsigned16 = 4,
        .unsigned64 = std::numeric_limits<uint64_t>::max(),
        .maybe_unsigned64 = std::numeric_limits<uint64_t>::min(),
        .signed8 = 8,
        .maybe_signed8 = 0,
        .signed64 = std::numeric_limits<int64_t>::max(),
        .maybe_signed64 = 0,
        .float32 = 1.2345,
        .maybe_float32 = 22.0 / 7.0,
        .float64 = 0.0,
        .maybe_float64 = 1.0,
        .coveralls = std::make_shared<Coveralls>("some_dict"),
        .test_trait = nullptr,
    };
}

coverall::SimpleDict coverall::create_none_dict() {
    return {};
}

uint64_t coverall::get_num_alive() {
    return NUM_ALIVE;
}

std::vector<std::shared_ptr<coverall::TestTrait>> coverall::get_traits() {
    return {};
}

coverall::MaybeSimpleDict coverall::get_maybe_simple_dict(int8_t index) {
    if (index == 0) {
        return { coverall::MaybeSimpleDict::YEAH {} };
    }
    else if (index == 1) {
        return { coverall::MaybeSimpleDict::NAH {} };
    }

    throw std::runtime_error("invalid index");
}

coverall::SimpleFlatMacroEnum coverall::get_simple_flat_macro_enum(int8_t index) {
    if (index == 0) {
        return { coverall::SimpleFlatMacroEnum::FIRST { "the first" } };
    }
    else if (index == 1) {
        return { coverall::SimpleFlatMacroEnum::SECOND { 2 } };
    }

    throw std::runtime_error("invalid index");
}

void coverall::println(std::string text) {
    std::cout << text << std::endl;
}

void coverall::throw_macro_error() {
    throw coverall::coverall_macro_error::TooManyMacros();
}

void coverall::throw_flat_error() {
    auto error = coverall::coverall_flat_error::TooManyVariants();
    error.num = 99;

    throw error;
}

void coverall::throw_flat_macro_error() {
    auto error = coverall::coverall_flat_macro_error::TooManyVariants();
    error.num = 88;

    throw error;
}

void coverall::throw_rich_error_no_variant_data() {
    throw coverall::coverall_rich_error_no_variant_data::TooManyPlainVariants();
}

void coverall::throw_complex_macro_error() {
    auto error = coverall::complex_macro_error::OsError();
    error.code = 1;
    error.extended_code = 2;

    throw error;
}

#include <coverall_cpp_scaffolding.cpp>
