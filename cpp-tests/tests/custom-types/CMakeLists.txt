add_executable(custom-types-test main.cpp ${BINDINGS_SRC_DIR}/custom_types.cpp)
add_test(NAME custom-types-test COMMAND custom-types-test)