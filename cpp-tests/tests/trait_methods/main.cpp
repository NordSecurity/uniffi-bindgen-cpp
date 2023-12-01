#include "test_common.hpp"

#include <trait_methods.hpp>

void test_trait_methods() {
    auto trait = trait_methods::TraitMethods::init("trait1");
    ASSERT_EQ(trait->to_string(), "TraitMethods(trait1)");
    ASSERT_EQ(trait->to_debug_string(), "TraitMethods { val: \"trait1\" }");
    ASSERT_EQ(trait->hash(), 8148112548604738188);

    auto ptr_copy = trait;
    auto trait_copy = trait_methods::TraitMethods::init("trait1");
    ASSERT_EQ(trait->to_string(), trait_copy->to_string());
    ASSERT_EQ(trait->to_debug_string(), trait_copy->to_debug_string());
    ASSERT_EQ(trait->hash(), trait_copy->hash());

    // Two different shared ptr's should differ
    ASSERT_NE(trait, trait_copy);

    // Two shared ptr's pointing to the same object should be equal
    ASSERT_EQ(trait, ptr_copy);

    // Internal equality check should work
    ASSERT_TRUE(trait->eq(trait_copy));
    ASSERT_FALSE(trait->ne(trait_copy));

    auto trait2 = trait_methods::TraitMethods::init("trait2");
    ASSERT_NE(trait, trait2);
    ASSERT_FALSE(trait->eq(trait2));
    ASSERT_TRUE(trait->ne(trait2));

    ASSERT_NE(trait->hash(), trait2->hash());
    ASSERT_NE(trait->to_string(), trait2->to_string());
    ASSERT_NE(trait->to_debug_string(), trait2->to_debug_string());
}

void test_proc_methods() {
    auto trait = trait_methods::ProcTraitMethods::init("trait1");
    ASSERT_EQ(trait->to_string(), "ProcTraitMethods(trait1)");
    ASSERT_EQ(trait->to_debug_string(), "ProcTraitMethods { val: \"trait1\" }");
    ASSERT_EQ(trait->hash(), 8148112548604738188);

    auto ptr_copy = trait;
    auto trait_copy = trait_methods::ProcTraitMethods::init("trait1");
    ASSERT_EQ(trait->to_string(), trait_copy->to_string());
    ASSERT_EQ(trait->to_debug_string(), trait_copy->to_debug_string());
    ASSERT_EQ(trait->hash(), trait_copy->hash());

    // Two different shared ptr's should differ
    ASSERT_NE(trait, trait_copy);

    // Two shared ptr's pointing to the same object should be equal
    ASSERT_EQ(trait, ptr_copy);

    // Internal equality check should work
    ASSERT_TRUE(trait->eq(trait_copy));
    ASSERT_FALSE(trait->ne(trait_copy));

    auto trait2 = trait_methods::ProcTraitMethods::init("trait2");
    ASSERT_NE(trait, trait2);
    ASSERT_FALSE(trait->eq(trait2));
    ASSERT_TRUE(trait->ne(trait2));

    ASSERT_NE(trait->hash(), trait2->hash());
    ASSERT_NE(trait->to_string(), trait2->to_string());
    ASSERT_NE(trait->to_debug_string(), trait2->to_debug_string());
}

int main() {
    test_trait_methods();
    test_proc_methods();

    return 0;
}