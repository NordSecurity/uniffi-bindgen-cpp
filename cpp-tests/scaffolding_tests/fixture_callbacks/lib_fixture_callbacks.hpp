#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace {
    namespace fixture_callbacks {
        class SimpleError: public std::runtime_error {
        public:
            SimpleError() : std::runtime_error("") {}
            SimpleError(const std::string &what_arg) : std::runtime_error(what_arg) {}
        };

        namespace simple_error {
            class BadArgument : public SimpleError {
            public:
                BadArgument() : SimpleError("") {}
                BadArgument(const std::string &what_arg) : SimpleError(what_arg) {}
            };

            class UnexpectedError : public SimpleError {
            public:
                UnexpectedError() : SimpleError("") {}
                UnexpectedError(const std::string &what_arg) : SimpleError(what_arg) {}
            };
        }

        class ComplexError: public std::runtime_error {
        public:
            ComplexError() : std::runtime_error("") {}
            ComplexError(const std::string &what_arg) : std::runtime_error(what_arg) {}
        };

        namespace complex_error {
            class ReallyBadArgument : public ComplexError {
            public:
                ReallyBadArgument() : ComplexError("") {}
                ReallyBadArgument(const std::string &what_arg) : ComplexError(what_arg) {}

                uint32_t code;
            };

            class UnexpectedErrorWithReason : public ComplexError {
            public:
                UnexpectedErrorWithReason() : ComplexError("") {}
                UnexpectedErrorWithReason(const std::string &what_arg) : ComplexError(what_arg), reason(what_arg) {}

                std::string reason;
            };
        }

        class ForeignGetters {
        public:
            virtual ~ForeignGetters() = default;

            virtual bool get_bool(bool v, bool argument_two) = 0;
            virtual std::string get_string(std::string v, bool argument_two) = 0;
            virtual std::optional<std::string> get_option(std::optional<std::string> v, bool argument_two) = 0;
            virtual std::vector<int32_t> get_list(std::vector<int32_t> v, bool argument_two) = 0;
            virtual void get_nothing(std::string v) = 0;
        };

        class RustGetters {
        public:
             RustGetters() = default;

            bool get_bool(std::shared_ptr<ForeignGetters> foreign, bool v, bool argument_two);
            std::string get_string(std::shared_ptr<ForeignGetters> foreign, std::string v, bool argument_two);
            std::optional<std::string> get_option(std::shared_ptr<ForeignGetters> foreign, std::optional<std::string> v, bool argument_two);
            std::vector<int32_t> get_list(std::shared_ptr<ForeignGetters> foreign, std::vector<int32_t> v, bool argument_two);
            std::optional<std::string> get_string_optional_callback(std::shared_ptr<ForeignGetters> callback, std::string v, bool argument_two);
            void get_nothing(std::shared_ptr<ForeignGetters> foreign, std::string v);
        };

        class StoredForeignStringifier {
        public:
            virtual ~StoredForeignStringifier() = default;

            virtual std::string from_simple_type(int32_t v) = 0;
            virtual std::string from_complex_type(std::optional<std::vector<std::optional<double>>> values) = 0;
        };

        class RustStringifier {
        public:
                RustStringifier(std::shared_ptr<StoredForeignStringifier> foreign) : foreign(foreign) {}

                std::string from_simple_type(uint32_t v);
                std::string from_complex_type(std::optional<std::vector<std::optional<double>>> values);
        private:
            std::shared_ptr<StoredForeignStringifier> foreign;
        };
    }
}
