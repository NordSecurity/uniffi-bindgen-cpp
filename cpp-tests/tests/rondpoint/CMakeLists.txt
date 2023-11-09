add_executable(rondpoint-test main.cpp ${BINDINGS_SRC_DIR}/rondpoint.cpp)
add_test(NAME rondpoint-test COMMAND rondpoint-test)