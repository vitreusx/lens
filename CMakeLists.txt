cmake_minimum_required(VERSION 3.16)
project(lens LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

add_subdirectory(glad)
add_subdirectory(glm)

add_library(obj INTERFACE)
target_include_directories(obj
    INTERFACE
    OBJ-Loader/Source)

add_library(stb)
target_sources(stb
    PRIVATE
    src/stb_image_impl.cpp)
target_include_directories(stb
    PUBLIC
    stb)

add_executable(lens)
target_sources(lens
    PRIVATE
    src/main.cpp
    src/window.h
    src/shader.h
    src/program.h
    src/vao.h
    src/buffer.h
    src/camera.h
    src/model.h
    src/random.h
    src/texture.h
    src/storage_buffer.h
    src/gravity.h)

target_link_libraries(lens
    PUBLIC
    glfw glad glm obj stb)

set_target_properties(lens
    PROPERTIES
    LINKER_LANGUAGE CXX
    LANGUAGE CXX)

file(COPY res/ DESTINATION res/)

add_executable(defl test/defl.cpp)
