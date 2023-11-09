#include <test_common.hpp>

#include <rondpoint.hpp>
#include <variant>

int main() {
    // Copy
    auto dict = rondpoint::Dictionnaire {
        .un = rondpoint::Enumeration::UN,
        .deux = true,
        .petit_nombre = 0,
        .gros_nombre = 123456789u
    };
    auto copied_dict = rondpoint::copie_dictionnaire(dict);
    ASSERT_EQ(dict.un, copied_dict.un);
    ASSERT_EQ(dict.deux, copied_dict.deux);
    ASSERT_EQ(dict.petit_nombre, copied_dict.petit_nombre);
    ASSERT_EQ(dict.gros_nombre, copied_dict.gros_nombre);

    ASSERT_EQ(rondpoint::Enumeration::DEUX, rondpoint::copie_enumeration(rondpoint::Enumeration::DEUX));

    auto vec = { rondpoint::Enumeration::UN, rondpoint::Enumeration::DEUX };
    auto copied_vec = rondpoint::copie_enumerations(vec);
    ASSERT_EQ(vec.size(), copied_vec.size());
    ASSERT_TRUE(std::equal(vec.begin(), vec.end(), copied_vec.begin()));

    ASSERT_TRUE(rondpoint::switcheroo(false));

    using Variant = std::variant<rondpoint::EnumerationAvecDonnees::ZERO, rondpoint::EnumerationAvecDonnees::UN, rondpoint::EnumerationAvecDonnees::DEUX>;
    auto z = rondpoint::EnumerationAvecDonnees::ZERO {};
    auto u = rondpoint::EnumerationAvecDonnees::UN {.premier = 2};
    auto d = rondpoint::EnumerationAvecDonnees::DEUX {.premier= 1, .second = "test"};

    auto map = std::unordered_map<std::string, rondpoint::EnumerationAvecDonnees> {
        { "zero",  {Variant(z)}},
        { "un",  {Variant(u)}},
        { "deux", {Variant(d)} }
    };
    auto copied_map = rondpoint::copie_carte(map);
    ASSERT_EQ(map.size(), copied_map.size());

    auto stringifier = rondpoint::Stringifier::init();
    auto meanValue = 0x1234'5678'9123'4567;
    
    ASSERT_EQ("uniffi 💚 cpp!", stringifier->well_known_string("cpp")) ;

    return 0;
}