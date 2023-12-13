#pragma once

#include <string>

struct CustomString {
    std::string string;

    CustomString(const std::string &string): string(string) { }

    std::string to_string() const {
        return this->string;
    }
};