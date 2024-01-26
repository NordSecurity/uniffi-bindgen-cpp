#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace {
    namespace custom_fixture_callbacks {
        class ForeignGetters {
        public:
            virtual bool get_bool(bool v, bool arg2) = 0;
            virtual std::string get_string(std::string v, bool arg2) = 0;
            virtual std::vector<int32_t> get_list(std::vector<int32_t> v, bool arg2) = 0;
            virtual std::optional<std::string> get_option(std::optional<std::string> v, bool arg2) = 0;
            virtual std::vector<uint8_t> get_bytes(std::vector<uint8_t> v, bool arg2) = 0;

            virtual ~ForeignGetters() = default;
        };

        bool get_bool_roundtrip(std::shared_ptr<ForeignGetters> cb, bool b, bool arg2);
        std::string get_string_roundtrip(std::shared_ptr<ForeignGetters> cb, std::string s, bool arg2);
        std::vector<int32_t> get_list_roundtrip(std::shared_ptr<ForeignGetters> cb, std::vector<int32_t> l, bool arg2);
        std::optional<std::string> get_option_roundtrip(std::shared_ptr<ForeignGetters> cb, std::optional<std::string> o, bool arg2);
        std::optional<std::string> get_string_optional_callback(std::shared_ptr<ForeignGetters> cb, std::string v, bool arg2);
        std::vector<uint8_t> get_bytes_roundtrip(std::shared_ptr<ForeignGetters> cb, std::vector<uint8_t> v, bool arg2);
    } // namespace custom_fixture_callbacks
} // namespace
