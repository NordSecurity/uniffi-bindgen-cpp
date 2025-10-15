cmake_minimum_required(VERSION 3.10)
project(binding-tests VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

find_package(Threads REQUIRED)

set(BINDINGS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../target/debug)
set(BINDINGS_SRC_DIR ${BINDINGS_BUILD_DIR}/bindings)

if (WIN32)
    set(UNIFFI_FIXTURES_LIB uniffi_bindgen_cpp_fixtures.dll)
    set(ADDITIONAL_LIBS ws2_32 userenv advapi32 ntdll crypt32 Bcrypt)
elseif(UNIX AND NOT APPLE)
    set(UNIFFI_FIXTURES_LIB libuniffi_bindgen_cpp_fixtures.so)
    set(ADDITIONAL_LIBS)
else()
    set(UNIFFI_FIXTURES_LIB libuniffi_bindgen_cpp_fixtures.dylib)
    set(ADDITIONAL_LIBS)
endif()

find_program(VALGRIND "valgrind")
macro(memcheck_test TEST_NAME)
if (VALGRIND)
    add_test(NAME ${TEST_NAME}-test-memcheck
    COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite $<TARGET_FILE:${TEST_NAME}>)
endif()
endmacro(memcheck_test)

# Add a bindings test case
macro(test_case TEST_NAME)
add_executable(${TEST_NAME}-test tests/${TEST_NAME}/main.cpp ${BINDINGS_SRC_DIR}/${TEST_NAME}.cpp)

target_include_directories(${TEST_NAME}-test PRIVATE ${BINDINGS_SRC_DIR} include)
target_link_directories(${TEST_NAME}-test PRIVATE ${BINDINGS_BUILD_DIR})
target_link_libraries(${TEST_NAME}-test uniffi_bindgen_cpp_fixtures Threads::Threads ${ADDITIONAL_LIBS})
target_compile_definitions(${TEST_NAME}-test PRIVATE UNIFFI_BINDING_DIR="${BINDINGS_SRC_DIR}")

add_test(NAME ${TEST_NAME}-test COMMAND ${TEST_NAME}-test)
memcheck_test(${TEST_NAME}-test)

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
test_case(enum_style_test)
test_case(empty_type)
test_case(error_types_builtin)
test_case(reserved_field_name)

# Special multilib test that only needs to compile to check for potential symbol clashes
add_executable(multilib-test 
    tests/multilib/main.cpp
    ${BINDINGS_SRC_DIR}/arithmetic.cpp
    ${BINDINGS_SRC_DIR}/coverall.cpp
    ${BINDINGS_SRC_DIR}/sprites.cpp
)
target_include_directories(multilib-test PRIVATE ${BINDINGS_SRC_DIR} include)
target_link_directories(multilib-test PRIVATE ${BINDINGS_BUILD_DIR})
target_link_libraries(multilib-test uniffi_bindgen_cpp_fixtures Threads::Threads ${ADDITIONAL_LIBS})
add_dependencies(multilib-test bindings)

add_custom_target(libs ALL
    BYPRODUCTS ${BINDING_FILES}
    COMMAND cargo build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
    COMMENT "Generating libs"
)

add_custom_target(bindings ALL
    DEPENDS libs
    COMMAND ./uniffi-bindgen-cpp --library ${UNIFFI_FIXTURES_LIB} --out-dir bindings
    WORKING_DIRECTORY ${BINDINGS_BUILD_DIR}
    COMMENT "Generating bindings"
)
