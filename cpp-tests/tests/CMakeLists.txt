# Enable warnings and treat them as errors
# add_compile_options(
#     $<$<CXX_COMPILER_ID:MSVC>:/W4>
#     $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic
# )
# set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include_directories(${BINDINGS_SRC_DIR} include)
link_directories(${BINDINGS_BUILD_DIR})

link_libraries(uniffi_bindgen_cpp_fixtures)

add_subdirectory(arithmetic)
add_subdirectory(geometry)
add_subdirectory(sprites)
add_subdirectory(callback)
add_subdirectory(chronological)
add_subdirectory(todolist)
add_subdirectory(traits)
add_subdirectory(custom-types)