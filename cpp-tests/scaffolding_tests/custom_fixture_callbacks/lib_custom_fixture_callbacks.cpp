#include "lib_custom_fixture_callbacks.hpp"
#include <optional>

bool custom_fixture_callbacks::get_bool_roundtrip(std::shared_ptr<custom_fixture_callbacks::ForeignGetters> cb, bool v, bool args2) { return cb->get_bool(v, args2); }

std::string custom_fixture_callbacks::get_string_roundtrip(std::shared_ptr<custom_fixture_callbacks::ForeignGetters> cb, std::string v, bool args2) {
    return cb->get_string(v, args2);
}

std::vector<int32_t> custom_fixture_callbacks::get_list_roundtrip(std::shared_ptr<custom_fixture_callbacks::ForeignGetters> cb, std::vector<int32_t> v, bool args2) {
    return cb->get_list(v, args2);
}

std::optional<std::string> custom_fixture_callbacks::get_option_roundtrip(std::shared_ptr<custom_fixture_callbacks::ForeignGetters> cb, std::optional<std::string> v, bool args2) {
    return cb->get_option(v, args2);
}

std::optional<std::string> custom_fixture_callbacks::get_string_optional_callback(std::shared_ptr<custom_fixture_callbacks::ForeignGetters> cb, std::string v, bool args2) {
    return cb ? std::make_optional(cb->get_string(v, args2)) : std::nullopt;
}

std::vector<uint8_t> custom_fixture_callbacks::get_bytes_roundtrip(std::shared_ptr<custom_fixture_callbacks::ForeignGetters> cb, std::vector<uint8_t> v, bool args2) {
    return cb->get_bytes(v, args2);
}

#include <custom_fixture_callbacks_cpp_scaffolding.cpp>
