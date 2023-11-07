#include <test_common.hpp>

#include <callbacks.hpp>

class CallAnswererImpl: public callbacks::CallAnswerer {
public:
    std::string mode;

    CallAnswererImpl(const std::string& mode) : mode(std::move(mode)) {}

    std::string answer() override {
        if (mode.compare("normal") == 0) {
            return "Hello";
        } else if (mode.compare("busy") == 0) {
            throw callbacks::Busy("I'm busy");
        } else {
            throw std::runtime_error("Unknown mode");
        }
    }
};

int main() {
    auto telephone = callbacks::Telephone::init();
    ASSERT_EQ(telephone->call(std::make_shared<CallAnswererImpl>("normal")), "Hello");
    EXPECT_EXCEPTION(telephone->call(std::make_shared<CallAnswererImpl>("busy")), callbacks::Busy);
    EXPECT_EXCEPTION(telephone->call(std::make_shared<CallAnswererImpl>("unknown")), callbacks::InternalTelephoneError);    

    return 0;
}