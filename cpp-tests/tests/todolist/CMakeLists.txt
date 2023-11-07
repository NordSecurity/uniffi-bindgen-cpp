add_executable(todolist-test main.cpp ${BINDINGS_SRC_DIR}/todolist.cpp)
add_test(NAME todolist-test COMMAND todolist-test)