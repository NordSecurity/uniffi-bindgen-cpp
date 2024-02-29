#include "lib_arithmetic.hpp"

uint64_t arithmetic::add(uint64_t a, uint64_t b) {
    return a + b;
}

uint64_t arithmetic::sub(uint64_t a, uint64_t b) {
    if (a < b) {
        throw arithmetic_error::IntegerOverflow();
    }

    return a - b;
}

uint64_t arithmetic::div(uint64_t a, uint64_t b) {
    if (b == 0) {
        throw std::runtime_error("division by zero");
    }

    return a / b;
}

int8_t arithmetic::equal(uint64_t a, uint64_t b) {
    return a == b;
}

#include <arithmetic_cpp_scaffolding.cpp>
