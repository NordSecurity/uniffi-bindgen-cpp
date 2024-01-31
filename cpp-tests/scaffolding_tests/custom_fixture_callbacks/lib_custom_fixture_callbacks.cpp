#include "lib_custom_fixture_callbacks.hpp"
#include <optional>

bool custom_fixture_callbacks::NativeGetters::get_bool(std::shared_ptr<ForeignGetters> cb, bool v, bool arg2) {
    return cb->get_bool(v, arg2);
}

std::string custom_fixture_callbacks::NativeGetters::get_string(std::shared_ptr<ForeignGetters> cb, std::string v, bool arg2) {
    return cb->get_string(v, arg2);
}

std::optional<std::string> custom_fixture_callbacks::NativeGetters::get_option(std::shared_ptr<ForeignGetters> cb, std::optional<std::string> v, bool arg2) {
    return cb->get_option(v, arg2);
}

std::vector<int32_t> custom_fixture_callbacks::NativeGetters::get_list(std::shared_ptr<ForeignGetters> cb, std::vector<int32_t> v, bool arg2) {
    return cb->get_list(v, arg2);
}

std::vector<uint8_t> custom_fixture_callbacks::NativeGetters::get_bytes(std::shared_ptr<ForeignGetters> cb, std::vector<uint8_t> v, bool arg2) {
    return cb->get_bytes(v, arg2);
}

std::optional<std::string> custom_fixture_callbacks::NativeGetters::get_string_optional_callback(std::shared_ptr<ForeignGetters> cb, std::string v, bool arg2) {
    if (cb) {
        return cb->get_option(v, arg2);
    } else {
        return std::nullopt;
    }
}

void custom_fixture_callbacks::NativeGetters::get_nothing(std::shared_ptr<ForeignGetters> cb, std::string v) {
    cb->get_nothing(v);
}

std::string custom_fixture_callbacks::NativeStringifier::from_simple_type(int32_t value) {
    return cb->from_simple_type(value);
}

std::string custom_fixture_callbacks::NativeStringifier::from_complex_type(std::optional<std::vector<std::optional<double>>> values) {
    return cb->from_complex_type(values);
}

#include <custom_fixture_callbacks_cpp_scaffolding.cpp>
