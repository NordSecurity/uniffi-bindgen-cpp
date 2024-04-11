#include <string>
#include <cstdint>
#include <vector>
#include <optional>
#include <variant>
#include <unordered_map>

namespace {
    namespace rondpoint {
        struct MinusculeMajusculeDict {
            bool minuscule_majuscule_field;
        };

        enum class MinusculeMajusculeEnum {
            kMinusculeMajusculeVariant
        };

        enum class Enumeration {
            kUn,
            kDeux,
            kTrois
        };

        class EnumerationAvecDonnees {
        public:
            struct kZero {};
            struct kUn {
                uint32_t premier;
            };
            struct kDeux {
                uint32_t premier;
                std::string second;
            };

            EnumerationAvecDonnees(EnumerationAvecDonnees::kZero variant) { this->variant = variant; }
            EnumerationAvecDonnees(EnumerationAvecDonnees::kUn variant) { this->variant = variant; }
            EnumerationAvecDonnees(EnumerationAvecDonnees::kDeux variant) { this->variant = variant; }


            std::variant<kZero, kUn, kDeux> variant;
        };

        struct Dictionnaire {
            Enumeration un;
            bool deux;
            uint8_t petit_nombre;
            uint64_t gros_nombre;
        };

        struct DictionnaireNombres {
            uint8_t petit_nombre;
            uint16_t court_nombre;
            uint32_t nombre_simple;
            uint64_t gros_nombre;
        };

        struct DictionnaireNombresSignes {
            int8_t petit_nombre;
            int16_t court_nombre;
            int32_t nombre_simple;
            int64_t gros_nombre;
        };

        struct OptionneurDictionnaire {
            int8_t i8_var = -8;
            uint8_t u8_var = 8;

            int16_t i16_var = -0x10;
            uint16_t u16_var = 0x10;

            int32_t i32_var = -32;
            uint32_t u32_var = 32;

            int64_t i64_var = -64;
            uint64_t u64_var = 64;

            float float_var = 4.0f;
            double double_var = 8.0;

            bool boolean_var = true;

            std::string string_var = "default";
            std::vector<std::string> list_var = {};

            Enumeration enumeration_var = Enumeration::kDeux;
            std::optional<MinusculeMajusculeEnum> dictionnaire_var = std::nullopt;
        };

        class Retourneur {
        public:
            Retourneur() = default;

            int8_t identique_i8(int8_t value);
            uint8_t identique_u8(uint8_t value);

            int16_t identique_i16(int16_t value);
            uint16_t identique_u16(uint16_t value);

            int32_t identique_i32(int32_t value);
            uint32_t identique_u32(uint32_t value);

            int64_t identique_i64(int64_t value);
            uint64_t identique_u64(uint64_t value);

            float identique_float(float value);
            double identique_double(double value);

            bool identique_boolean(bool value);

            std::string identique_string(std::string value);

            DictionnaireNombresSignes identique_nombres_signes(DictionnaireNombresSignes value);
            DictionnaireNombres identique_nombres(DictionnaireNombres value);
            OptionneurDictionnaire identique_optionneur_dictionnaire(OptionneurDictionnaire value);
        };

        class Stringifier {
        public:
            Stringifier() = default;

            std::string well_known_string(std::string value);

            std::string to_string_i8(int8_t value);
            std::string to_string_u8(uint8_t value);

            std::string to_string_i16(int16_t value);
            std::string to_string_u16(uint16_t value);

            std::string to_string_i32(int32_t value);
            std::string to_string_u32(uint32_t value);

            std::string to_string_i64(int64_t value);
            std::string to_string_u64(uint64_t value);

            std::string to_string_float(float value);
            std::string to_string_double(double value);
            std::string to_string_boolean(bool value);
        };

        class Optionneur {
        public:
            Optionneur() = default;

            bool sinon_boolean(bool value);
            std::string sinon_string(std::string value);

            std::vector<std::string> sinon_sequence(std::vector<std::string> value);

            std::optional<std::string> sinon_null(std::optional<std::string> value);
            std::optional<int32_t> sinon_zero(std::optional<int32_t> value);

            uint8_t sinon_u8_dec(uint8_t value);
            int8_t sinon_i8_dec(int8_t value);

            uint16_t sinon_u16_dec(uint16_t value);
            int16_t sinon_i16_dec(int16_t value);

            uint32_t sinon_u32_dec(uint32_t value);
            int32_t sinon_i32_dec(int32_t value);

            uint64_t sinon_u64_dec(uint64_t value);
            int64_t sinon_i64_dec(int64_t value);

            uint8_t sinon_u8_hex(uint8_t value);
            int8_t sinon_i8_hex(int8_t value);

            uint16_t sinon_u16_hex(uint16_t value);
            int16_t sinon_i16_hex(int16_t value);

            uint32_t sinon_u32_hex(uint32_t value);
            int32_t sinon_i32_hex(int32_t value);

            uint64_t sinon_u64_hex(uint64_t value);
            int64_t sinon_i64_hex(int64_t value);

            uint32_t sinon_u32_oct(uint32_t value);

            float sinon_f32(float value);
            double sinon_f64(double value);

            Enumeration sinon_enum(Enumeration value);
        };

        Dictionnaire copie_dictionnaire(Dictionnaire d);
        Enumeration copie_enumeration(Enumeration e);

        std::vector<Enumeration> copie_enumerations(std::vector<Enumeration> e);
        std::unordered_map<std::string, EnumerationAvecDonnees> copie_carte(std::unordered_map<std::string, EnumerationAvecDonnees> c);

        bool switcheroo(bool b);
    }
}
