#include "test_common.hpp"

#include <type_flattening.hpp>

int main() {
    int value = 42;

    auto structure = type_flattening::get_struct(value);
    ASSERT_NE(structure.optional_arc, nullptr);
    ASSERT_EQ(structure.optional_arc->get_value(), value);

    auto roundtrip = type_flattening::struct_roundtrip(structure);
    ASSERT_NE(roundtrip.optional_arc, nullptr);
    ASSERT_EQ(roundtrip.optional_arc->get_value(), value);

    return 0;
}
