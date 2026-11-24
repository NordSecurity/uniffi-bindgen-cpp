#include <test_common.hpp>

#include <error_types_builtin.hpp>

// A C++ implementation of a trait whose method returns an error type. Rust calls back into
// this, and the generated vtable lowers the returned value, which is a `shared_ptr` here
// while the converter takes a `const &`.
struct Producer : error_types_builtin::ErrorProducer {
    std::shared_ptr<error_types_builtin::TupleError> produce() override {
        auto value = std::make_shared<error_types_builtin::tuple_error::Value>();
        value->v1 = 7;
        return value;
    }
};

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

    try {
        error_types_builtin::oops_tuple(1);
        ASSERT_TRUE(false);
    } catch(error_types_builtin::TupleError& e) {
        ASSERT_EQ("value: 1", e.describe());
    } catch(...) {
        ASSERT_TRUE(false);
    }
    

    try {
        error_types_builtin::oops_custom(7);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::TupleError& e) {
        auto subtype = dynamic_cast<error_types_builtin::tuple_error::Value*>(&e);
        ASSERT_TRUE(subtype != nullptr);
        ASSERT_EQ(subtype->v1, 7);
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_custom_object();
        ASSERT_TRUE(false);
    } catch (error_types_builtin::ErrorInterface& e) {
        ASSERT_EQ(std::vector<std::string> {"custom-object-oops"}, e.chain());
    } catch (...) {
        ASSERT_TRUE(false);
    }

    auto produced = error_types_builtin::describe_produced(std::make_shared<Producer>());
    ASSERT_EQ("value: 7", produced);

    try {
        error_types_builtin::oops_payload(0);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::payload_error::Bad &e) {
        ASSERT_EQ(std::get<error_types_builtin::Payload::kCircle>(e.payload.get_variant()).radius, 1.5);
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_payload(1);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::payload_error::BadWrapped &e) {
        ASSERT_EQ(e.wrapped.name, "wrapped");
        ASSERT_EQ(std::get<error_types_builtin::Payload::kLabel>(e.wrapped.payload.get_variant()).v1, "label");
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_payload(2);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::payload_error::BadTuple &e) {
        ASSERT_EQ(std::get<error_types_builtin::Payload::kCircle>(e.v1.get_variant()).radius, 2.5);
        ASSERT_EQ(e.v2, 7u);
    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        error_types_builtin::oops_payload(3);
        ASSERT_TRUE(false);
    } catch (error_types_builtin::payload_error::Plain &e) {
        ASSERT_EQ(e.code, 42u);
    } catch (...) {
        ASSERT_TRUE(false);
    }

    auto bad = std::make_shared<error_types_builtin::payload_error::Bad>(error_types_builtin::Payload(error_types_builtin::Payload::kLabel{"built"}));
    auto bad_back = std::dynamic_pointer_cast<error_types_builtin::payload_error::Bad>(
        error_types_builtin::payload_error_roundtrip(bad));
    ASSERT_TRUE(bad_back != nullptr);
    ASSERT_EQ(std::get<error_types_builtin::Payload::kLabel>(bad_back->payload.get_variant()).v1, "built");

    auto wrapped = std::make_shared<error_types_builtin::payload_error::BadWrapped>(
        error_types_builtin::Wrapped{"w", error_types_builtin::Payload(error_types_builtin::Payload::kCircle{3.0})});
    auto wrapped_back = std::dynamic_pointer_cast<error_types_builtin::payload_error::BadWrapped>(
        error_types_builtin::payload_error_roundtrip(wrapped));
    ASSERT_TRUE(wrapped_back != nullptr);
    ASSERT_EQ(wrapped_back->wrapped.name, "w");
    ASSERT_EQ(std::get<error_types_builtin::Payload::kCircle>(wrapped_back->wrapped.payload.get_variant()).radius, 3.0);

    auto tuple = std::make_shared<error_types_builtin::payload_error::BadTuple>(error_types_builtin::Payload(error_types_builtin::Payload::kCircle{4.0}), 3);
    auto tuple_back = std::dynamic_pointer_cast<error_types_builtin::payload_error::BadTuple>(
        error_types_builtin::payload_error_roundtrip(tuple));
    ASSERT_TRUE(tuple_back != nullptr);
    ASSERT_EQ(std::get<error_types_builtin::Payload::kCircle>(tuple_back->v1.get_variant()).radius, 4.0);
    ASSERT_EQ(tuple_back->v2, 3u);

    auto plain = std::make_shared<error_types_builtin::payload_error::Plain>();
    plain->code = 9;
    auto plain_back = std::dynamic_pointer_cast<error_types_builtin::payload_error::Plain>(
        error_types_builtin::payload_error_roundtrip(plain));
    ASSERT_TRUE(plain_back != nullptr);
    ASSERT_EQ(plain_back->code, 9u);
    ASSERT_EQ(std::string(error_types_builtin::payload_error::Plain("message").what()), "message");

    return 0;
}
