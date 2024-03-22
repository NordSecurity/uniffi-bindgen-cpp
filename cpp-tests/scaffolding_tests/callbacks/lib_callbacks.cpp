#include "lib_callbacks.hpp"

std::string callbacks::Telephone::call(std::shared_ptr<CallAnswerer> answerer)  {
    try {
        return answerer->answer();
    } catch (telephone_error::Busy& e) {
        throw e;
    } catch (std::runtime_error&) {
        throw telephone_error::InternalTelephoneError();
    }
}

#include <callbacks_cpp_scaffolding.cpp>
