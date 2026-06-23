cmake_minimum_required(VERSION 3.20)

project(papyrus)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build libraries as static by default" FORCE)

add_subdirectory(third_party/SDL)
add_subdirectory(third_party/SDL_ttf)
add_subdirectory(third_party/googletest)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()

add_executable(papyrus
    src/main.cpp
    src/Application.h
    src/Application.cpp
)

target_include_directories(papyrus PRIVATE src)

add_subdirectory(src/Rendering)
add_subdirectory(src/Views)
add_subdirectory(src/Editor)
add_subdirectory(src/FileBrowser)
add_subdirectory(src/Search)
add_subdirectory(src/Syntax)
add_subdirectory(src/Terminal)
add_subdirectory(src/Core)
add_subdirectory(src/Screensaver)

target_link_libraries(papyrus
    PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

add_subdirectory(tests)
