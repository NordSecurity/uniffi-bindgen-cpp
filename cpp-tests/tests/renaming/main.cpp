#include "test_common.hpp"

#include <renaming.hpp>

void test_renaming()
{
    // Renamed record, and a renamed field on it.
    auto record = renaming::cpp_function(21);
    ASSERT_EQ(21, record.cpp_field);

    // Renamed object, with a renamed method and argument.
    auto object = renaming::CppObject::init(20);
    ASSERT_EQ(22, object->cpp_method(2));

    // Renamed enum variant, and a renamed field inside it.
    auto value = renaming::make_rust_enum();
    auto variant = std::get<renaming::CppEnum::kCppVariant>(value.get_variant());
    ASSERT_EQ(7, variant.cpp_variant_field);
}

int main()
{
    test_renaming();

    return 0;
}
