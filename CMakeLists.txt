cmake_minimum_required(VERSION 3.16)
project(lens)
set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

add_subdirectory(glad)
add_subdirectory(glm)

add_executable(lens src/main.cpp)
target_link_libraries(lens
    PUBLIC
    glfw
    glad
    glm)

configure_file(shader.vert shader.vert COPYONLY)
configure_file(shader.frag shader.frag COPYONLY)