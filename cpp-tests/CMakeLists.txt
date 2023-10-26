cmake_minimum_required(VERSION 3.5)
project(binding-tests VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

set(BINDINGS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../target/debug)
set(BINDINGS_SRC_DIR ${BINDINGS_BUILD_DIR}/bindings)

add_custom_target(build-wrappers ALL
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Wrapper build target"    
)
add_custom_command(
    TARGET build-wrappers
    COMMAND cargo build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
    COMMENT "Building generator and libs"
)
add_custom_command(
    TARGET build-wrappers
    COMMAND ./bindgen --library libuniffi_bindgen_cpp_fixtures.so --out-dir bindings
    WORKING_DIRECTORY ${BINDINGS_BUILD_DIR}
    COMMENT "Generating bindings"
)
add_subdirectory(tests)
