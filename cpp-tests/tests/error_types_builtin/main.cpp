#include <test_common.hpp>

#include <error_types_builtin.hpp>

int main() {
    try {
        error_types_builtin::oops();
        ASSERT_TRUE(false);
    } catch (error_types_builtin::ErrorInterface& e) {
        ASSERT_EQ(e.to_string(), "because uniffi told me so\n\nCaused by:\n    oops");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops();
        ASSERT_TRUE(false);
    } catch (error_types_builtin::ErrorInterface& e) {
        ASSERT_EQ(e.to_string(), "because uniffi told me so\n\nCaused by:\n    oops");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops();
        ASSERT_TRUE(false);
    } catch (error_types_builtin::ErrorInterface& e) {
        auto expected_chain = std::vector<std::string>{ "because uniffi told me so", "oops" };
        ASSERT_EQ(e.chain(), expected_chain);
        ASSERT_EQ(e.link(0), "because uniffi told me so");
        ASSERT_EQ(e.to_debug_string(), "ErrorInterface { e: because uniffi told me so\n\nCaused by:\n    oops }");
        ASSERT_EQ(e.to_string(), "because uniffi told me so\n\nCaused by:\n    oops");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::toops();
        ASSERT_TRUE(false);
    } catch (error_types_builtin::ErrorTrait& e) {
        ASSERT_EQ(e.msg(), "trait-oops");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    {
        auto e = error_types_builtin::get_error("the error");
        auto expected_chain = std::vector<std::string>{ "the error" };
        ASSERT_EQ(e->chain(), expected_chain);
        ASSERT_EQ(e->to_debug_string(), "ErrorInterface { e: the error }");
        ASSERT_EQ(e->to_string(), "the error");
    }

    try {
        error_types_builtin::throw_rich("oh no");
        ASSERT_TRUE(false);
    } catch (error_types_builtin::RichError& e) {
        ASSERT_EQ(e.to_debug_string(), "RichError { e: \"oh no\" }");
        ASSERT_EQ(e.to_string(), "RichError: \"oh no\"");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::TestInterface::fallible_new();
        ASSERT_TRUE(false);
    } catch (error_types_builtin::ErrorInterface& e) {
        ASSERT_EQ(e.to_string(), "fallible_new");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    {
        auto interface = error_types_builtin::TestInterface::init();
        try {
            interface->oops();
            ASSERT_TRUE(false);
        } catch (error_types_builtin::ErrorInterface& e) {
            ASSERT_EQ(e.to_string(), "because the interface told me so\n\nCaused by:\n    oops");
        } catch (...) {
            ASSERT_TRUE(false);
        }
    }

    try {
        error_types_builtin::throw_proc_error("eek");
        ASSERT_TRUE(false);
    } catch (error_types_builtin::ProcErrorInterface& e) {
        ASSERT_EQ(e.message(), "eek");
        ASSERT_EQ(e.to_string(), "ProcErrorInterface(eek)");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_enum(0);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::error::Oops& e) {
      ASSERT_EQ(e.what(), std::string(""));
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_enum(1);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::error::Value& e) {
        ASSERT_EQ(e.value, "value");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_enum(2);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::error::IntValue& e) {
        ASSERT_EQ(e.value, 2);
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_enum(3);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::error::FlatInnerError& e) {
        auto subtype = dynamic_cast<error_types_builtin::flat_inner::CaseA*>(e.error.get());
        ASSERT_TRUE(subtype != nullptr);
        ASSERT_EQ(std::string(e.error->what()), "inner");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_enum(4);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::error::FlatInnerError& e) {
        auto subtype = dynamic_cast<error_types_builtin::flat_inner::CaseB*>(e.error.get());
        ASSERT_TRUE(subtype != nullptr);
        ASSERT_EQ(std::string(e.error->what()), "NonUniffiTypeValue: value");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_enum(5);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::error::InnerError& e) {
        auto subtype = dynamic_cast<error_types_builtin::inner::CaseA*>(e.error.get());
        ASSERT_TRUE(subtype != nullptr);
        ASSERT_EQ(subtype->v1, "inner");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    {
        auto r = error_types_builtin::get_tuple(nullptr);
        auto subtype = dynamic_cast<error_types_builtin::tuple_error::Oops*>(r.get());
        ASSERT_TRUE(subtype != nullptr);
        ASSERT_EQ(subtype->v1, "oops");
    }

    try {
        error_types_builtin::oops_tuple(0);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::TupleError& e) {
        auto subtype = dynamic_cast<error_types_builtin::tuple_error::Oops*>(&e);
        ASSERT_TRUE(subtype != nullptr);
        ASSERT_EQ(subtype->v1, "oops");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_tuple(1);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::TupleError& e) {
        auto subtype = dynamic_cast<error_types_builtin::tuple_error::Value*>(&e);
        ASSERT_TRUE(subtype != nullptr);
        ASSERT_EQ(subtype->v1, 1);
    } catch (...) {
        ASSERT_TRUE(false);
    }

    return 0;
}
