#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

namespace {
    namespace custom_fixture_callbacks {
        typedef std::optional<std::vector<std::optional<double>>> ComplexType;

        enum class Enumeration {
            A = 1,
            B,
            C,
            UNKNOWN,
        };

        class ForeignGetters {
        public:
            virtual bool get_bool(bool v, bool arg2) = 0;
            virtual std::string get_string(std::string v, bool arg2) = 0;
            virtual std::optional<std::string> get_option(std::optional<std::string> v, bool arg2) = 0;
            virtual std::vector<int32_t> get_list(std::vector<int32_t> v, bool arg2) = 0;
            virtual std::vector<uint8_t> get_bytes(std::vector<uint8_t> v, bool arg2) = 0;
            virtual void get_nothing(std::string v) = 0;
            virtual Enumeration get_enum(Enumeration v, uint32_t variant, bool arg2) = 0;

            virtual ~ForeignGetters() = default;
        };

        class StoredForeignStringifier {
        public:
            virtual std::string from_simple_type(int32_t value) = 0;
            virtual std::string from_complex_type(std::optional<std::vector<std::optional<double>>> values) = 0;

            virtual ~StoredForeignStringifier() = default;
        };

        class NativeGetters {
        public:
            bool get_bool(std::shared_ptr<ForeignGetters> cb, bool v, bool arg2);
            std::string get_string(std::shared_ptr<ForeignGetters> cb, std::string v, bool arg2);
            std::optional<std::string> get_option(std::shared_ptr<ForeignGetters> cb, std::optional<std::string> v, bool arg2);
            std::vector<int32_t> get_list(std::shared_ptr<ForeignGetters> cb, std::vector<int32_t> v, bool arg2);
            std::vector<uint8_t> get_bytes(std::shared_ptr<ForeignGetters> cb, std::vector<uint8_t> v, bool arg2);
            std::optional<std::string> get_string_optional_callback(std::shared_ptr<ForeignGetters> cb, std::string v, bool arg2);
            void get_nothing(std::shared_ptr<ForeignGetters> cb, std::string v);
            Enumeration get_enum(std::shared_ptr<ForeignGetters> cb, Enumeration v, uint32_t variant, bool arg2);
        };

        class NativeStringifier {
        public:
            NativeStringifier(std::shared_ptr<StoredForeignStringifier> cb) : cb(cb) {}

            std::string from_simple_type(int32_t value);
            std::string from_complex_type(std::optional<std::vector<std::optional<double>>> values);
        private:
            std::shared_ptr<StoredForeignStringifier> cb;
        };
    } // namespace custom_fixture_callbacks
} // namespace
