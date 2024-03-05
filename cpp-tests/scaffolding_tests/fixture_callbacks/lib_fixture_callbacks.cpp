#include "lib_fixture_callbacks.hpp"

bool fixture_callbacks::RustGetters::get_bool(std::shared_ptr<fixture_callbacks::ForeignGetters> foreign, bool v, bool argument_two) {
    return foreign->get_bool(v, argument_two);
}

std::string fixture_callbacks::RustGetters::get_string(std::shared_ptr<fixture_callbacks::ForeignGetters> foreign, std::string v, bool argument_two) {
    try {
        return foreign->get_string(v, argument_two);
    } catch (const std::runtime_error &e) {
        throw fixture_callbacks::simple_error::UnexpectedError(e.what());
    }
}

std::optional<std::string> fixture_callbacks::RustGetters::get_option(std::shared_ptr<fixture_callbacks::ForeignGetters> foreign, std::optional<std::string> v, bool argument_two) {
    try {
        return foreign->get_option(v, argument_two);
    } catch (const fixture_callbacks::complex_error::ReallyBadArgument &e) {
        throw e;
    } catch (const fixture_callbacks::complex_error::UnexpectedErrorWithReason &e) {
        throw fixture_callbacks::complex_error::UnexpectedErrorWithReason(e.reason);
    } catch (const std::runtime_error &e) {
        throw fixture_callbacks::complex_error::UnexpectedErrorWithReason(e.what());
    }
}

std::vector<int32_t> fixture_callbacks::RustGetters::get_list(std::shared_ptr<fixture_callbacks::ForeignGetters> foreign, std::vector<int32_t> v, bool argument_two) {
    return foreign->get_list(v, argument_two);
}

std::optional<std::string> fixture_callbacks::RustGetters::get_string_optional_callback(std::shared_ptr<fixture_callbacks::ForeignGetters> foreign, std::string v, bool argument_two) {
    if (foreign) {
        try {
            return foreign->get_string(v, argument_two);
        } catch (const fixture_callbacks::simple_error::BadArgument &e) {
            throw e;
        } catch (const std::runtime_error &e) {
            throw fixture_callbacks::simple_error::UnexpectedError();
        }
    }

    return std::nullopt;
}

void fixture_callbacks::RustGetters::get_nothing(std::shared_ptr<fixture_callbacks::ForeignGetters> foreign, std::string v) {
    try {
        return foreign->get_nothing(v);
    } catch (const fixture_callbacks::simple_error::BadArgument &e) {
        throw e;
    } catch (const std::runtime_error &e) {
        throw fixture_callbacks::simple_error::UnexpectedError(e.what());
    }
}

std::string fixture_callbacks::RustStringifier::from_simple_type(uint32_t v) {
    return this->foreign->from_simple_type(v);
}

std::string fixture_callbacks::RustStringifier::from_complex_type(std::optional<std::vector<std::optional<double>>> values) {
    return this->foreign->from_complex_type(values);
}


#include <fixture_callbacks_cpp_scaffolding.cpp>
