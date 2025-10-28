#include "test_common.hpp"

#include <reserved_field_name.hpp>

int main() {
    reserved_field_name::Foo foo = {42, 42, 42};
    reserved_field_name::assert_fields_are_42(foo);

    return 0;
}
