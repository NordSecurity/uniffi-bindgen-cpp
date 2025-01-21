#include "test_common.hpp"

#include <fstream>
#include <regex>

#include <uniffi_docstring.hpp>

// TODO: Current checking implementation cannot find the `<second-line>` entry
const std::vector<std::string> expected_docstrings = {
    "<docstring-namespace>",
    "<docstring-function>",
    "<docstring-multiline-function>",
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

    std::regex docstring_reg("\\<docstring-(.*?)\\>");
    std::smatch match;
    std::vector<std::string> extracted_docstrings;

    while (std::regex_search(source, match, docstring_reg)) {
        extracted_docstrings.push_back(match[0].str());
        source = match.suffix().str();
    }

    for (const auto& docstring : expected_docstrings) {
        if (std::find(extracted_docstrings.begin(), extracted_docstrings.end(), docstring) == extracted_docstrings.end()) {
            std::cerr << "Could not find expected docstring: " << docstring << std::endl;
            ASSERT_TRUE(false);
        }
    }

    for (const auto& docstring : extracted_docstrings) {
        if (std::find(expected_docstrings.begin(), expected_docstrings.end(), docstring) == expected_docstrings.end()) {
            std::cerr << "Unexpected docstring found: " << docstring << std::endl;
            ASSERT_TRUE(false);
        }
    }

    return 0;
}
