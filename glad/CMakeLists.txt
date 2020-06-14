add_library(glad)

target_include_directories(glad
    PUBLIC
    include)

target_sources(glad
    PRIVATE
    src/glad.cpp)

set_target_properties(glad
    PROPERTIES
    LINKER_LANGUAGE CXX
    LANGUAGE CXX)
