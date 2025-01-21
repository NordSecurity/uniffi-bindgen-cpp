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
            throw callbacks::telephone_error::Busy("I'm busy");
        } else {
            throw std::runtime_error("Unknown mode");
        }
    }
};

class DiscountSim: public callbacks::SimCard {
public:
    std::string name() {
        return "C++";
    }
};

int main() {
    auto telephone = callbacks::Telephone::init();
    auto default_sim = callbacks::get_sim_cards()[0];

    ASSERT_EQ(telephone->call(default_sim, std::make_shared<CallAnswererImpl>("normal")), "Hello");
    EXPECT_EXCEPTION(telephone->call(default_sim, std::make_shared<CallAnswererImpl>("busy")), callbacks::telephone_error::Busy);
    EXPECT_EXCEPTION(telephone->call(default_sim, std::make_shared<CallAnswererImpl>("unknown")), callbacks::telephone_error::InternalTelephoneError);
    ASSERT_EQ(telephone->call(std::make_shared<DiscountSim>(), std::make_shared<CallAnswererImpl>("normal")), "C++ est bon marché");

    return 0;
}
