#include <limits>
#include <test_common.hpp>

#include <rondpoint.hpp>
#include <variant>
#include <string>

template <typename T, typename F, typename ...Args>
void affirm_aller_retour(const T& c, const F& func, Args&& ...arg) {
    auto fn = std::mem_fn(func);
    for (auto&& arg : {std::forward<Args>(arg)...}) {
        ASSERT_EQ(arg, fn(c, arg));
    }
}

template <typename T, typename F, typename ...Args>
void affirm_enchaine(const T& c, const F& func, Args&& ...arg) {
    auto fn = std::mem_fn(func);
    for (auto&& arg : {std::forward<Args>(arg)...}) {
        std::string arg_str = std::to_string(arg);
        std::string ret_str = fn(c, arg);

        ASSERT_TRUE((arg_str == ret_str));
    }
}

void test_copy() {
    auto dict = rondpoint::Dictionnaire {
        .un = rondpoint::Enumeration::kDeux,
        .deux = true,
        .petit_nombre = 0,
        .gros_nombre = 123456789u
    };
    auto copied_dict = rondpoint::copie_dictionnaire(dict);
    ASSERT_EQ(dict.un, copied_dict.un);
    ASSERT_EQ(dict.deux, copied_dict.deux);
    ASSERT_EQ(dict.petit_nombre, copied_dict.petit_nombre);
    ASSERT_EQ(dict.gros_nombre, copied_dict.gros_nombre);

    auto list = std::vector<rondpoint::Enumeration> { rondpoint::Enumeration::kUn, rondpoint::Enumeration::kDeux};
    auto copied_list = rondpoint::copie_enumerations(list);
    ASSERT_EQ(list.size(), copied_list.size());
    ASSERT_TRUE(std::equal(list.begin(), list.end(), copied_list.begin()));

    auto map = std::unordered_map<std::string, rondpoint::EnumerationAvecDonnees> {
        { "zero",  rondpoint::EnumerationAvecDonnees::kZero {}},
        { "un",  rondpoint::EnumerationAvecDonnees::kUn {.premier = 2}},
        { "deux", rondpoint::EnumerationAvecDonnees::kDeux {.premier= 1, .second = "test"}}
    };
    auto copied_map = rondpoint::copie_carte(map);
    ASSERT_EQ(map.size(), copied_map.size());
    for (auto& [key, value] : map) {
        ASSERT_EQ(value.get_variant().index(), copied_map.at(key).get_variant().index());
        auto v = value;
        std::visit([&v](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, rondpoint::EnumerationAvecDonnees::kZero>) {
            } else if constexpr (std::is_same_v<T, rondpoint::EnumerationAvecDonnees::kUn>) {
                ASSERT_EQ(2, arg.premier);
            } else if constexpr (std::is_same_v<T, rondpoint::EnumerationAvecDonnees::kDeux>) {
                ASSERT_EQ(1, arg.premier);
                ASSERT_EQ("test", arg.second);
            }
        }, value.get_variant());
    }

    ASSERT_TRUE(rondpoint::switcheroo(false));
}

void test_roundtrip() {
    auto rt = rondpoint::Retourneur::init();

    auto mean_value = 0x1234'5678'9123'4567;

    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_boolean, true, false);

    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_i8, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max(), (int8_t)mean_value);
    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_u8, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max(), (uint8_t)mean_value);

    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_i16, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max(), (int16_t)mean_value);
    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_u16, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max(), (uint16_t)mean_value);

    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_i32, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max(), (int32_t)mean_value);
    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_u32, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max(), (uint32_t)mean_value);

    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_i64, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max(), (int64_t)mean_value);
    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_u64, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max(), (uint64_t)mean_value);

    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_float, std::numeric_limits<float>::min(), std::numeric_limits<float>::max(), std::numeric_limits<float>::epsilon());
    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_double, std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), std::numeric_limits<double>::epsilon());

    affirm_aller_retour(rt, &rondpoint::Retourneur::identique_string,
    "",
    "abc",
    "null\u0000byte",
    "été",
    "ښي لاس ته لوستلو لوستل",
    "😻emoji 👨‍👧‍👦multi-emoji, 🇨🇭a flag, a canal, panama"
    );

    for (auto i : {-1, 0, 1}) {
        auto nombres_signes = rondpoint::DictionnaireNombresSignes {
            .petit_nombre = (int8_t)i,
            .court_nombre = (int16_t)i,
            .nombre_simple = (int32_t)i,
            .gros_nombre = (int64_t)i,
        };
        auto ret = rt->identique_nombres_signes(nombres_signes);

        ASSERT_EQ(nombres_signes.petit_nombre, ret.petit_nombre);
        ASSERT_EQ(nombres_signes.court_nombre, ret.court_nombre);
        ASSERT_EQ(nombres_signes.nombre_simple, ret.nombre_simple);
        ASSERT_EQ(nombres_signes.gros_nombre, ret.gros_nombre);
    }

    for (auto i : {0, 1}) {
        auto nombres = rondpoint::DictionnaireNombres {
            .petit_nombre = (uint8_t)i,
            .court_nombre = (uint16_t)i,
            .nombre_simple = (uint32_t)i,
            .gros_nombre = (uint64_t)i,
        };
        auto ret = rt->identique_nombres(nombres);

        ASSERT_EQ(nombres.petit_nombre, ret.petit_nombre);
        ASSERT_EQ(nombres.court_nombre, ret.court_nombre);
        ASSERT_EQ(nombres.nombre_simple, ret.nombre_simple);
        ASSERT_EQ(nombres.gros_nombre, ret.gros_nombre);
    }
}

void test_stringifier() {
    auto st = rondpoint::Stringifier::init();

    auto mean_value = 0x1234'5678'9123'4567;

    ASSERT_EQ("uniffi 💚 cpp!", st->well_known_string("cpp"));

    // to_string doesn't work on booleans
    ASSERT_EQ("true", st->to_string_boolean(true));
    ASSERT_EQ("false", st->to_string_boolean(false));

    affirm_enchaine(st, &rondpoint::Stringifier::to_string_i8, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max(), (int8_t)mean_value);
    affirm_enchaine(st, &rondpoint::Stringifier::to_string_u8, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max(), (uint8_t)mean_value);

    affirm_enchaine(st, &rondpoint::Stringifier::to_string_i16, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max(), (int16_t)mean_value);
    affirm_enchaine(st, &rondpoint::Stringifier::to_string_u16, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max(), (uint16_t)mean_value);

    affirm_enchaine(st, &rondpoint::Stringifier::to_string_i32, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max(), (int32_t)mean_value);
    affirm_enchaine(st, &rondpoint::Stringifier::to_string_u32, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max(), (uint32_t)mean_value);

    affirm_enchaine(st, &rondpoint::Stringifier::to_string_i64, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max(), (int64_t)mean_value);
    affirm_enchaine(st, &rondpoint::Stringifier::to_string_u64, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max(), (uint64_t)mean_value);

    for (float f : {std::numeric_limits<float>::min(), std::numeric_limits<float>::max(), std::numeric_limits<float>::epsilon()}) {
        ASSERT_TRUE((std::abs(f - std::stof(st->to_string_float(f))) <= std::numeric_limits<float>::epsilon()));
    }

    for (double d : {std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), std::numeric_limits<double>::epsilon()}) {
        ASSERT_TRUE((std::abs(d - std::stod(st->to_string_double(d))) <= std::numeric_limits<double>::epsilon()));
    }
}

void test_default_parameter_literals_in_record() {
    auto default_dict = rondpoint::OptionneurDictionnaire {};
    ASSERT_EQ(default_dict.i8_var, -8);
    ASSERT_EQ(default_dict.u8_var, 8);
    ASSERT_EQ(default_dict.i16_var, -16);
    ASSERT_EQ(default_dict.u16_var, 0x10);
    ASSERT_EQ(default_dict.i32_var, -32);
    ASSERT_EQ(default_dict.u32_var, 32);
    ASSERT_EQ(default_dict.i64_var, -64);
    ASSERT_EQ(default_dict.u64_var, 64);
    ASSERT_EQ(default_dict.float_var, 4.0f);
    ASSERT_EQ(default_dict.double_var, 8.0);
    ASSERT_EQ(default_dict.boolean_var, true);
    ASSERT_EQ(default_dict.string_var, "default");
    ASSERT_EQ(default_dict.list_var, std::vector<std::string>{});
    ASSERT_EQ(default_dict.enumeration_var, rondpoint::Enumeration::kDeux);
    ASSERT_EQ(default_dict.dictionnaire_var, std::nullopt);

    auto rt = rondpoint::Retourneur::init();
    auto dict = rondpoint::OptionneurDictionnaire {
        .i8_var = -8,
        .u8_var = 8,
        .i16_var = -16,
        .u16_var = 0x10,
        .i32_var = -32,
        .u32_var = 32,
        .i64_var = -64,
        .u64_var = 64,
        .float_var = 4.0f,
        .double_var = 8.0,
        .boolean_var = true,
        .string_var = "default",
        .list_var = std::vector<std::string>{},
        .enumeration_var = rondpoint::Enumeration::kDeux,
        .dictionnaire_var = std::nullopt
    };
    auto copied_dict = rt->identique_optionneur_dictionnaire(dict);
    ASSERT_EQ(dict.i8_var, copied_dict.i8_var);
    ASSERT_EQ(dict.u8_var, copied_dict.u8_var);
    ASSERT_EQ(dict.i16_var, copied_dict.i16_var);
    ASSERT_EQ(dict.u16_var, copied_dict.u16_var);
    ASSERT_EQ(dict.i32_var, copied_dict.i32_var);
    ASSERT_EQ(dict.u32_var, copied_dict.u32_var);
    ASSERT_EQ(dict.i64_var, copied_dict.i64_var);
    ASSERT_EQ(dict.u64_var, copied_dict.u64_var);
    ASSERT_EQ(dict.float_var, copied_dict.float_var);
    ASSERT_EQ(dict.double_var, copied_dict.double_var);
    ASSERT_EQ(dict.boolean_var, copied_dict.boolean_var);
    ASSERT_EQ(dict.string_var, copied_dict.string_var);
    ASSERT_EQ(dict.list_var, copied_dict.list_var);
    ASSERT_EQ(dict.enumeration_var, copied_dict.enumeration_var);
    ASSERT_EQ(dict.dictionnaire_var, copied_dict.dictionnaire_var);
}

int main() {
    test_copy();
    test_roundtrip();
    test_stringifier();
    test_default_parameter_literals_in_record();

    return 0;
}
