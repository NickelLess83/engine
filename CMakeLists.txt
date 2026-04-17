cmake_minimum_required(VERSION 3.20)
project(engine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# SDL2_mixer — prefer pkg-config since it ships no cmake config on most distros
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

# GLAD vendored static library
add_library(glad STATIC third_party/glad/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(engine_lib STATIC ${ENGINE_SOURCES})

target_include_directories(engine_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
)

target_link_libraries(engine_lib
    PUBLIC
        ${SDL2_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        OpenGL::GL
        glad
        glm::glm
)

# Define ENGINE_DEBUG for all non-Release builds
target_compile_definitions(engine_lib PUBLIC
    $<$<NOT:$<CONFIG:Release>>:ENGINE_DEBUG>
)

add_subdirectory(game)
