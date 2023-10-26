add_executable(arithmetic-test main.cpp ${BINDINGS_SRC_DIR}/arithmetic.cpp)
add_test(NAME arithmetic-test COMMAND arithmetic-test)