cmake_minimum_required(VERSION 3.20)

project(papyrus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(third_party/SDL)
add_subdirectory(third_party/SDL_ttf)


add_executable(papyrus
    src/main.cpp
    src/Renderer.cpp
    src/Renderer.h
    src/util.h
)

target_link_libraries(papyrus
    PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)