cmake_minimum_required(VERSION 3.5)
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

# Add a scaffolding test case
macro(scaffolding_test_case TEST_NAME)

add_executable(${TEST_NAME}-scaffolding-test tests/${TEST_NAME}/main.cpp ${BINDINGS_SRC_DIR}/${TEST_NAME}.cpp)
target_include_directories(${TEST_NAME}-scaffolding-test PRIVATE ${BINDINGS_SRC_DIR} include)
target_link_libraries(${TEST_NAME}-scaffolding-test uniffi_fixtures Threads::Threads)

add_test(NAME ${TEST_NAME}-scaffolding-test COMMAND ${TEST_NAME}-scaffolding-test)
memcheck_test(${TEST_NAME}-scaffolding-test)

add_dependencies(${TEST_NAME}-scaffolding-test bindings scaffolding)

list(APPEND SCAFFOLDING_FILES ${BINDINGS_SRC_DIR}/${TEST_NAME}_cpp_scaffolding.cpp)
list(APPEND SCAFFOLDING_LIB_FILES scaffolding_tests/${TEST_NAME}/lib_${TEST_NAME}.cpp)
list(APPEND SCAFFOLDING_LIB_INCLUDE_DIRS scaffolding_tests/${TEST_NAME})

endmacro(scaffolding_test_case)

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

# scaffolding_test_case(arithmetic)
# scaffolding_test_case(callbacks)
# scaffolding_test_case(fixture_callbacks)
# scaffolding_test_case(chronological)
# scaffolding_test_case(custom_types)
# scaffolding_test_case(geometry)
# scaffolding_test_case(rondpoint)
# scaffolding_test_case(sprites)
# scaffolding_test_case(todolist)
# scaffolding_test_case(traits)
# scaffolding_test_case(coverall)
# scaffolding_test_case(custom_types_builtin)
# scaffolding_test_case(enum_style_test)
# scaffolding_test_case(empty_type)

# add_library(uniffi_fixtures SHARED
#     ${SCAFFOLDING_LIB_FILES})
# target_include_directories(uniffi_fixtures PRIVATE
#     ${SCAFFOLDING_LIB_INCLUDE_DIRS} ${BINDINGS_SRC_DIR})
# add_dependencies(uniffi_fixtures scaffolding)

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

# add_custom_target(scaffolding ALL
#     BYPRODUCTS ${SCAFFOLDING_FILES}
#     DEPENDS libs
#     COMMAND ./uniffi-bindgen-cpp --scaffolding --library ${UNIFFI_FIXTURES_LIB} --out-dir bindings
#     WORKING_DIRECTORY ${BINDINGS_BUILD_DIR}
#     COMMENT "Generating scaffolding"
# )
