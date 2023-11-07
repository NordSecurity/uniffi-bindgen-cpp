add_executable(sprite-test main.cpp ${BINDINGS_SRC_DIR}/sprites.cpp)
add_test(NAME sprite-test COMMAND sprite-test)