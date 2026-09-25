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

void test_record_traits() {
    auto recordA1 = trait_methods::TraitRecord { .s = "a", .i = 1 };
    auto recordA2 = trait_methods::TraitRecord { .s = "a", .i = 2 };
    auto recordB1 = trait_methods::TraitRecord { .s = "b", .i = 1 };

    ASSERT_TRUE(recordA1.eq(recordA2));
    ASSERT_FALSE(recordA1.ne(recordA2));
    ASSERT_EQ(0, recordA1.cmp(recordA2));
    ASSERT_EQ(recordA1.hash(), recordA2.hash());

    ASSERT_FALSE(recordA1.eq(recordB1));
    ASSERT_TRUE(recordA1.ne(recordB1));
    ASSERT_EQ(-1, recordA1.cmp(recordB1));
    ASSERT_EQ(1, recordB1.cmp(recordA1));
    ASSERT_NE(recordA1.hash(), recordB1.hash());

    ASSERT_EQ("TraitRecord { s: \"a\", i: 1 }", recordA1.to_debug_string());

    auto udlRecord1 = trait_methods::UdlRecord { .s = "a", .i = 1 };
    auto udlRecord2 = trait_methods::UdlRecord { .s = "a", .i = 2 };
    ASSERT_TRUE(udlRecord1.eq(udlRecord2));
    ASSERT_EQ(0, udlRecord1.cmp(udlRecord2));
    ASSERT_EQ(udlRecord1.hash(), udlRecord2.hash());
}

void test_enum_traits() {
    using TraitEnum = trait_methods::TraitEnum;

    auto variantS1 = TraitEnum(TraitEnum::kS { .v1 = "variantS1" });
    auto variantS2 = TraitEnum(TraitEnum::kS { .v1 = "variantS2" });
    auto variantI0 = TraitEnum(TraitEnum::kI { .v1 = 0 });
    auto variantN = TraitEnum(TraitEnum::kN {});

    ASSERT_TRUE(variantS1.eq(variantS2));
    ASSERT_FALSE(variantS1.ne(variantS2));
    ASSERT_EQ(0, variantS1.cmp(variantS2));
    ASSERT_EQ(variantS1.hash(), variantS2.hash());

    // Ordering between variants is S < I < N
    ASSERT_FALSE(variantS1.eq(variantI0));
    ASSERT_EQ(-1, variantS1.cmp(variantI0));
    ASSERT_EQ(1, variantI0.cmp(variantS1));
    ASSERT_EQ(-1, variantI0.cmp(variantN));
    ASSERT_NE(variantS1.hash(), variantI0.hash());

    ASSERT_EQ("TraitEnum::S(\"variantS1\")", variantS1.to_string());
    ASSERT_EQ("S(\"variantS1\")", variantS1.to_debug_string());
    ASSERT_EQ("TraitEnum::I(0)", variantI0.to_string());

    using UdlEnum = trait_methods::UdlEnum;
    auto udlVariantS1 = UdlEnum(UdlEnum::kS { .s = "variantS1" });
    auto udlVariantS2 = UdlEnum(UdlEnum::kS { .s = "variantS2" });
    auto udlVariantI0 = UdlEnum(UdlEnum::kI { .i = 0 });

    ASSERT_TRUE(udlVariantS1.eq(udlVariantS2));
    ASSERT_EQ(0, udlVariantS1.cmp(udlVariantS2));
    ASSERT_EQ(udlVariantS1.hash(), udlVariantS2.hash());
    ASSERT_EQ(-1, udlVariantS1.cmp(udlVariantI0));
}

void test_flat_enum_traits() {
    auto alpha = trait_methods::get_flat_trait_enum(0);
    auto beta = trait_methods::get_flat_trait_enum(1);

    ASSERT_EQ(trait_methods::FlatTraitEnum::kAlpha, alpha);
    ASSERT_EQ(trait_methods::FlatTraitEnum::kBeta, beta);

    ASSERT_EQ("FlatTraitEnum::flat-alpha", to_string(alpha));
    ASSERT_EQ("FlatTraitEnum::flat-beta", to_string(beta));
    ASSERT_EQ("Alpha", to_debug_string(alpha));
}

void test_error_traits() {
    // FlatError exports Display only.
    try
    {
        trait_methods::throw_flat_error(0);
        ASSERT_TRUE(false);
    }
    catch(trait_methods::FlatError &e)
    {
        ASSERT_EQ("error: not found", e.to_string());
    }

    // MultipleTraitError exports Debug, Display, Eq, Ord and Hash.
    try {
        trait_methods::throw_multiple_trait_error(0);
        ASSERT_TRUE(false);
    } catch (trait_methods::MultipleTraitError &noData) {
        ASSERT_EQ("MultipleTraitError::NoData", noData.to_string());
        ASSERT_TRUE(noData.eq(noData));
        ASSERT_FALSE(noData.ne(noData));
        ASSERT_EQ(0, noData.cmp(noData));

        try {
            trait_methods::throw_multiple_trait_error(1);
            ASSERT_TRUE(false);
        } catch (trait_methods::MultipleTraitError &nested) {
            ASSERT_FALSE(noData.eq(nested));
            ASSERT_EQ(-1, noData.cmp(nested));
            ASSERT_EQ(1, nested.cmp(noData));
            ASSERT_NE(noData.hash(), nested.hash());
        }
    }

    try {
        trait_methods::throw_api_failure(0);
        ASSERT_TRUE(false);
    } catch (trait_methods::ApiFailure &e) {
        ASSERT_EQ("api network issue", e.to_string());
    }
}

int main() {
    test_trait_methods();
    test_proc_methods();
    test_record_traits();
    test_enum_traits();
    test_flat_enum_traits();
    test_error_traits();

    return 0;
}