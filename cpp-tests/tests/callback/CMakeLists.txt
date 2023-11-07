add_executable(callback-test main.cpp ${BINDINGS_SRC_DIR}/callbacks.cpp)
add_test(NAME callback-test COMMAND callback-test)