add_executable(chronological-test main.cpp ${BINDINGS_SRC_DIR}/chronological.cpp)
add_test(NAME chronological-test COMMAND chronological-test)