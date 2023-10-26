add_executable(geometry-test main.cpp ${BINDINGS_SRC_DIR}/geometry.cpp)
add_test(NAME geometry-test COMMAND geometry-test)