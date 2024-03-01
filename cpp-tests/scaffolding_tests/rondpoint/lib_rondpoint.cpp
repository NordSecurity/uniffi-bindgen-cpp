#include "lib_rondpoint.hpp"

int8_t rondpoint::Retourneur::identique_i8(int8_t value) {
    return value;
}

uint8_t rondpoint::Retourneur::identique_u8(uint8_t value) {
    return value;
}

int16_t rondpoint::Retourneur::identique_i16(int16_t value) {
    return value;
}

uint16_t rondpoint::Retourneur::identique_u16(uint16_t value) {
    return value;
}

int32_t rondpoint::Retourneur::identique_i32(int32_t value) {
    return value;
}

uint32_t rondpoint::Retourneur::identique_u32(uint32_t value) {
    return value;
}

int64_t rondpoint::Retourneur::identique_i64(int64_t value) {
    return value;
}

uint64_t rondpoint::Retourneur::identique_u64(uint64_t value) {
    return value;
}

float rondpoint::Retourneur::identique_float(float value) {
    return value;
}

double rondpoint::Retourneur::identique_double(double value) {
    return value;
}

bool rondpoint::Retourneur::identique_boolean(bool value) {
    return value;
}

std::string rondpoint::Retourneur::identique_string(std::string value) {
    return value;
}

rondpoint::DictionnaireNombresSignes rondpoint::Retourneur::identique_nombres_signes(DictionnaireNombresSignes value) {
    return value;
}

rondpoint::DictionnaireNombres rondpoint::Retourneur::identique_nombres(DictionnaireNombres value) {
    return value;
}

rondpoint::OptionneurDictionnaire rondpoint::Retourneur::identique_optionneur_dictionnaire(OptionneurDictionnaire value) {
    return value;
}


std::string rondpoint::Stringifier::well_known_string(std::string value) {
    return "uniffi 💚 " + value + "!";
}

std::string rondpoint::Stringifier::to_string_i8(int8_t value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_u8(uint8_t value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_i16(int16_t value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_u16(uint16_t value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_i32(int32_t value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_u32(uint32_t value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_i64(int64_t value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_u64(uint64_t value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_float(float value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_double(double value) {
    return std::to_string(value);
}

std::string rondpoint::Stringifier::to_string_boolean(bool value) {
    return value ? "true" : "false";
}


bool rondpoint::Optionneur::sinon_boolean(bool value) {
    return value;
}

std::string rondpoint::Optionneur::sinon_string(std::string value) {
    return value;
}

std::vector<std::string> rondpoint::Optionneur::sinon_sequence(std::vector<std::string> value) {
    return value;
}

std::optional<std::string> rondpoint::Optionneur::sinon_null(std::optional<std::string> value) {
    return value;
}

std::optional<int32_t> rondpoint::Optionneur::sinon_zero(std::optional<int32_t> value) {
    return value;
}

uint8_t rondpoint::Optionneur::sinon_u8_dec(uint8_t value) {
    return value;
}

int8_t rondpoint::Optionneur::sinon_i8_dec(int8_t value) {
    return value;
}

uint16_t rondpoint::Optionneur::sinon_u16_dec(uint16_t value) {
    return value;
}

int16_t rondpoint::Optionneur::sinon_i16_dec(int16_t value) {
    return value;
}

uint32_t rondpoint::Optionneur::sinon_u32_dec(uint32_t value) {
    return value;
}

int32_t rondpoint::Optionneur::sinon_i32_dec(int32_t value) {
    return value;
}

uint64_t rondpoint::Optionneur::sinon_u64_dec(uint64_t value) {
    return value;
}

int64_t rondpoint::Optionneur::sinon_i64_dec(int64_t value) {
    return value;
}

uint8_t rondpoint::Optionneur::sinon_u8_hex(uint8_t value) {
    return value;
}

int8_t rondpoint::Optionneur::sinon_i8_hex(int8_t value) {
    return value;
}

uint16_t rondpoint::Optionneur::sinon_u16_hex(uint16_t value) {
    return value;
}

int16_t rondpoint::Optionneur::sinon_i16_hex(int16_t value) {
    return value;
}

uint32_t rondpoint::Optionneur::sinon_u32_hex(uint32_t value) {
    return value;
}

int32_t rondpoint::Optionneur::sinon_i32_hex(int32_t value) {
    return value;
}

uint64_t rondpoint::Optionneur::sinon_u64_hex(uint64_t value) {
    return value;
}

int64_t rondpoint::Optionneur::sinon_i64_hex(int64_t value) {
    return value;
}

uint32_t rondpoint::Optionneur::sinon_u32_oct(uint32_t value) {
    return value;
}

float rondpoint::Optionneur::sinon_f32(float value) {
    return value;
}

double rondpoint::Optionneur::sinon_f64(double value) {
    return value;
}

rondpoint::Enumeration rondpoint::Optionneur::sinon_enum(Enumeration value) {
    return value;
}

rondpoint::Dictionnaire rondpoint::copie_dictionnaire(Dictionnaire d) {
    return d;
}

rondpoint::Enumeration rondpoint::copie_enumeration(Enumeration e) {
    return e;
}

std::vector<rondpoint::Enumeration> rondpoint::copie_enumerations(std::vector<Enumeration> e) {
    return e;
}

std::unordered_map<std::string, rondpoint::EnumerationAvecDonnees> rondpoint::copie_carte(std::unordered_map<std::string, EnumerationAvecDonnees> c) {
    return c;
}

bool rondpoint::switcheroo(bool b) {
    return !b;
}

#include <rondpoint_cpp_scaffolding.cpp>
