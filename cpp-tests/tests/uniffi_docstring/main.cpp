#include "test_common.hpp"

#include <fstream>

#include <uniffi_docstring.hpp>

const std::vector<std::string> expected_docstrings = {
    "<docstring-namespace>",
    "<docstring-function>",
    "<docstring-enum>",
    "<docstring-enum-variant>",
    "<docstring-enum-variant-2>",
    "<docstring-associated-enum>",
    "<docstring-associated-enum-variant>",
    "<docstring-associated-enum-variant-2>",
    "<docstring-error>",
    "<docstring-error-variant>",
    "<docstring-error-variant-2>",
    "<docstring-associated-error>",
    "<docstring-associated-error-variant>",
    "<docstring-associated-error-variant-2>",
    "<docstring-object>",
    "<docstring-primary-constructor>",
    "<docstring-alternate-constructor>",
    "<docstring-method>",
    "<docstring-record>",
    "<docstring-record-field>",
    "<docstring-callback>",
    "<docstring-callback-method>",
};

int main() {
    auto stream = std::ifstream(UNIFFI_BINDING_DIR "/uniffi_docstring.hpp");
    ASSERT_TRUE(stream.is_open());

    auto source = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
    ASSERT_FALSE(source.empty());

    for (const auto& docstring : expected_docstrings) {
        auto found = source.find(docstring);
        if (found == std::string::npos) {
            std::cerr << "Could not find docstring: " << docstring << std::endl;
            ASSERT_TRUE(false);
        }
    }

    return 0;
}