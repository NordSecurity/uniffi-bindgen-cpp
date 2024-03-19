#include "lib_custom_types.hpp"

custom_types::CustomTypesDemo custom_types::get_custom_types_demo(std::optional<custom_types::CustomTypesDemo> demo) {
    return demo.value_or(custom_types::CustomTypesDemo{
        .url = "http://example.com/",
        .handle = 123,
        .time_interval_ms = 456000,
        .time_interval_sec_dbl = 456.0,
        .time_interval_sec_flt = 777.0f,
    });
}

custom_types::ExampleCustomType custom_types::get_example_custom_type() {
    return "abadidea";
}

#include <custom_types_cpp_scaffolding.cpp>
