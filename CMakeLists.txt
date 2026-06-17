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
    src/Renderer.cpp
    src/Renderer.h
    src/Editor.cpp
    src/Editor.h
    src/UndoManager.h
    src/UndoManager.cpp
    src/TextBuffer.cpp
    src/TextBuffer.h
    src/FileBrowser.h
    src/FileBrowser.cpp
    src/SearchSession.h
    src/SearchSession.cpp
    src/SearchEngine.h
    src/SearchEngine.cpp
    src/SyntaxHighlighter.h
    src/SyntaxHighlighter.cpp
    src/CppLexer.h
    src/CppLexer.cpp
    src/Terminal.h
    src/Terminal.cpp
    src/CommandProcessor.h
    src/CommandProcessor.cpp
    src/Argparser.h
    src/logger.h
    src/types.h
    src/util.h
)

target_link_libraries(papyrus
    PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

add_subdirectory(tests)
