# AzerothCore module: mod-realm-first-titles
cmake_minimum_required(VERSION 3.16)

set(MOD_REALM_FIRST_TITLES_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_library(mod-realm-first-titles INTERFACE)

target_sources(mod-realm-first-titles INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RealmFirstTitles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/realm_first_titles_loader.cpp
)

target_include_directories(mod-realm-first-titles INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mod-realm-first-titles INTERFACE
    game
)
