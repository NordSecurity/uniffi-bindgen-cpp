add_executable(traits-test main.cpp ${BINDINGS_SRC_DIR}/traits.cpp)
add_test(NAME traits-test COMMAND traits-test)