#include <lib_empty_type.hpp>

empty_type::Empty empty_type::get_empty_type() {
    return Empty {};
}

void empty_type::send_empty_type(Empty e) {}

#include <empty_type_cpp_scaffolding.cpp>
