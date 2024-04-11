#include <cstdint>
#include <stdexcept>
#include <string>
#include <optional>
#include <vector>
#include <variant>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <mutex>
#include <atomic>

namespace {
    namespace coverall {
        typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> timestamp;

        class IFirst {
        public:
            IFirst() = default;

            bool compare(std::shared_ptr<class ISecond> other);
        };

        class ISecond {
        public:
            ISecond() = default;

            bool compare(std::shared_ptr<IFirst> other);
        };

        class ThreadsafeCounter {
        public:
            ThreadsafeCounter() = default;

            void busy_wait(int32_t ms);
            int32_t increment_if_busy();
        private:
            std::atomic_bool is_busy;
            std::atomic_int32_t count;
        };

        class TestTrait {
        public:
            virtual std::string name() = 0;

            virtual uint64_t number(std::shared_ptr<TestTrait> self) = 0;
            virtual uint64_t strong_count(std::shared_ptr<TestTrait> self) = 0;

            virtual void take_other(std::shared_ptr<TestTrait> other) = 0;
            virtual std::shared_ptr<TestTrait> get_other() = 0;
        };

        enum class Color {
            kRed,
            kBlue,
            kGreen
        };

        class Patch {
        public:
            Patch(Color color) : color(color) {}

            Color get_color();
        private:
            Color color;
        };

        struct Repair {
            timestamp when;
            std::shared_ptr<Patch> patch;
        };

        class Coveralls {
        public:
            Coveralls(std::string name, bool should_fail);
            Coveralls(std::string name);

            ~Coveralls();

            std::string get_name();

            bool maybe_throw(bool should_throw);
            bool maybe_throw_into(bool should_throw);
            bool maybe_throw_complex(uint8_t input);

            void panic(std::string message);

            void fallible_panic(std::string message);

            uint64_t strong_count(const std::shared_ptr<Coveralls> &self);

            void take_other(const std::shared_ptr<Coveralls> &other);
            std::shared_ptr<Coveralls> get_other();
            void take_other_fallible(const std::shared_ptr<Coveralls> &self);
            void take_other_panic(const std::shared_ptr<Coveralls> &self, std::string message);

            std::shared_ptr<Coveralls> clone_me();

            std::string get_status(std::string status);

            std::unordered_map<std::string, uint64_t> get_dict(std::string key, uint64_t value);
            std::unordered_map<std::string, uint64_t> get_dict2(std::string key, uint64_t value);
            std::unordered_map<uint32_t, uint64_t> get_dict3(uint32_t key, uint64_t value);

            void add_patch(std::shared_ptr<Patch> patch);
            void add_repair(Repair repair);
            std::vector<Repair> get_repairs();

            std::vector<uint8_t> reverse(std::vector<uint8_t> value);
        private:
            std::string name;

            std::shared_ptr<Coveralls> other;
            std::mutex other_mutex;

            std::vector<Repair> repairs;
            std::mutex repairs_mutex;
        };

        struct SimpleDict {
            std::string text;
            std::optional<std::string> maybe_text;
            std::vector<uint8_t> some_bytes;
            std::optional<std::vector<uint8_t>> maybe_some_bytes;
            bool a_bool;
            std::optional<bool> maybe_a_bool;
            uint8_t unsigned8;
            std::optional<uint8_t> maybe_unsigned8;
            uint16_t unsigned16;
            std::optional<uint16_t> maybe_unsigned16;
            uint64_t unsigned64;
            std::optional<uint64_t> maybe_unsigned64;
            int8_t signed8;
            std::optional<int8_t> maybe_signed8;
            int64_t signed64;
            std::optional<int64_t> maybe_signed64;
            float float32;
            std::optional<float> maybe_float32;
            double float64;
            std::optional<double> maybe_float64;
            std::shared_ptr<Coveralls> coveralls;
            std::shared_ptr<TestTrait> test_trait;
        };

        struct DictWithDefaults {
            std::string name;
            std::optional<std::string> category;
            uint64_t integer;
        };

        class MaybeSimpleDict {
        public:
            struct kYeah {
                SimpleDict d;
            };

            struct kNah {};

            MaybeSimpleDict(MaybeSimpleDict::kYeah value) { this->variant = value; }
            MaybeSimpleDict(MaybeSimpleDict::kNah value) { this->variant = value; }

            std::variant<kYeah, kNah> variant;
        };

        class SimpleFlatEnum {
        public:
            struct kFirst {
                std::string val;
            };

            struct kSecond {
                uint16_t num;
            };

            SimpleFlatEnum(SimpleFlatEnum::kFirst value) { this->variant = value; }
            SimpleFlatEnum(SimpleFlatEnum::kSecond value) { this->variant = value; }

            std::variant<kFirst, kSecond> variant;
        };

        class SimpleFlatMacroEnum {
        public:
            struct kFirst {
                std::string val;
            };

            struct kSecond {
                uint16_t num;
            };

            SimpleFlatMacroEnum(SimpleFlatMacroEnum::kFirst value) { this->variant = value; }
            SimpleFlatMacroEnum(SimpleFlatMacroEnum::kSecond value) { this->variant = value; }

            std::variant<kFirst, kSecond> variant;
        };

        namespace coverall_error {
            class TooManyHoles: public std::runtime_error {
            public:
                TooManyHoles() : TooManyHoles("The coverall has too many holes") {}
                TooManyHoles(const std::string &what_arg) : std::runtime_error(what_arg) {}
            };
        }

        namespace coverall_macro_error {
            class TooManyHoles: public std::runtime_error {
            public:
                TooManyHoles() : TooManyHoles("") {}
                TooManyHoles(const std::string &what_arg) : std::runtime_error(what_arg) {}
            };

            class TooManyMacros: public std::runtime_error {
            public:
                TooManyMacros() : TooManyMacros("") {}
                TooManyMacros(const std::string &what_arg) : std::runtime_error(what_arg) {}
            };
        }

        namespace coverall_flat_error {
            class TooManyVariants: public std::runtime_error {
                public:
                    TooManyVariants() : TooManyVariants("") {}
                    TooManyVariants(const std::string &what_arg) : std::runtime_error(what_arg) {}

                    int16_t num;
            };
        }

        namespace coverall_flat_macro_error {
            class TooManyVariants: public std::runtime_error {
                public:
                    TooManyVariants() : TooManyVariants("") {}
                    TooManyVariants(const std::string &what_arg) : std::runtime_error(what_arg) {}

                    int16_t num;
            };
        }


        namespace coverall_rich_error_no_variant_data {
            class TooManyPlainVariants: public std::runtime_error {
                public:
                    TooManyPlainVariants() : TooManyPlainVariants("") {}
                    TooManyPlainVariants(const std::string &what_arg) : std::runtime_error(what_arg) {}
            };
        }

        namespace complex_error {
            class OsError: public std::runtime_error {
                public:
                    OsError() : OsError("") {}
                    OsError(const std::string &what_arg) : std::runtime_error(what_arg) {}

                    int16_t code;
                    int16_t extended_code;
            };

            class PermissionDenied: public std::runtime_error {
                public:
                    PermissionDenied() : PermissionDenied("") {}
                    PermissionDenied(const std::string &what_arg) : std::runtime_error(what_arg) {}

                    std::string reason;
            };

            class UnknownError: public std::runtime_error {
                public:
                    UnknownError() : UnknownError("") {}
                    UnknownError(const std::string &what_arg) : std::runtime_error(what_arg) {}
            };
        }

        namespace complex_macro_error {
            class OsError: public std::runtime_error {
                public:
                    OsError() : OsError("") {}
                    OsError(const std::string &what_arg) : std::runtime_error(what_arg) {}

                    int16_t code;
                    int16_t extended_code;
            };

            class PermissionDenied: public std::runtime_error {
                public:
                    PermissionDenied() : PermissionDenied("") {}
                    PermissionDenied(const std::string &what_arg) : std::runtime_error(what_arg) {}

                    std::string reason;
            };

            class UnknownError: public std::runtime_error {
                public:
                    UnknownError() : UnknownError("") {}
                    UnknownError(const std::string &what_arg) : std::runtime_error(what_arg) {}
            };
        }

        SimpleDict create_some_dict();
        SimpleDict create_none_dict();

        uint64_t get_num_alive();

        std::vector<std::shared_ptr<TestTrait>> get_traits();

        MaybeSimpleDict get_maybe_simple_dict(int8_t index);

        SimpleFlatMacroEnum get_simple_flat_macro_enum(int8_t index);

        void println(std::string text);

        void throw_macro_error();

        void throw_flat_error();
        void throw_flat_macro_error();

        void throw_rich_error_no_variant_data();
        void throw_complex_macro_error();
    }
}
