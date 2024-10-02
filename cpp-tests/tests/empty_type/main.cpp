#include "test_common.hpp"

#include <empty_type.hpp>

int main() {
    auto empty = empty_type::get_empty_type();

    empty_type::send_empty_type(empty);

    return 0;
}
