file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(game ${GAME_SOURCES})

target_link_libraries(game PRIVATE engine_lib)

target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
