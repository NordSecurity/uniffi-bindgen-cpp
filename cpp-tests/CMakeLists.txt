cmake_minimum_required(VERSION 3.5)
project(binding-tests VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

find_package(Threads REQUIRED)

set(BINDINGS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../target/debug)
set(BINDINGS_SRC_DIR ${BINDINGS_BUILD_DIR}/bindings)

macro(test_case TEST_NAME)
add_executable(${TEST_NAME}-test tests/${TEST_NAME}/main.cpp ${BINDINGS_SRC_DIR}/${TEST_NAME}.cpp)

target_include_directories(${TEST_NAME}-test PRIVATE ${BINDINGS_SRC_DIR} tests/include)
target_link_directories(${TEST_NAME}-test PRIVATE ${BINDINGS_BUILD_DIR})
target_link_libraries(${TEST_NAME}-test uniffi_bindgen_cpp_fixtures Threads::Threads)
target_compile_definitions(${TEST_NAME}-test PRIVATE UNIFFI_BINDING_DIR="${BINDINGS_SRC_DIR}")

add_test(NAME ${TEST_NAME}-test COMMAND ${TEST_NAME}-test)

add_test(NAME ${TEST_NAME}-test-memcheck
  COMMAND valgrind
    --error-exitcode=1
    --tool=memcheck
    --leak-check=full
    --errors-for-leak-kinds=definite
    --show-leak-kinds=definite $<TARGET_FILE:${TEST_NAME}-test>)

add_dependencies(${TEST_NAME}-test bindings)

list(APPEND BINDING_FILES ${BINDINGS_SRC_DIR}/${TEST_NAME}.cpp)

endmacro(test_case)

test_case(arithmetic)
test_case(callbacks)
test_case(fixture_callbacks)
test_case(chronological)
test_case(custom_types)
test_case(geometry)
test_case(rondpoint)
test_case(sprites)
test_case(todolist)
test_case(traits)
test_case(coverall)
test_case(uniffi_docstring)
test_case(trait_methods)
test_case(custom_types_builtin)

add_custom_target(libs ALL
    BYPRODUCTS ${BINDING_FILES}
    COMMAND cargo build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
    COMMENT "Generating libs"
)

add_custom_target(bindings ALL
    DEPENDS libs
    COMMAND ./uniffi-bindgen-cpp --library libuniffi_bindgen_cpp_fixtures.so --out-dir bindings
    WORKING_DIRECTORY ${BINDINGS_BUILD_DIR}
    COMMENT "Generating bindings"
)