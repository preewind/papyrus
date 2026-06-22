list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/third_party/googletest/cmake")
include(GoogleTest)

function(add_simple_test testName src)
    add_executable(papyrus_${testName}
        ${src}
        ${ARGN}
    )

    target_include_directories(papyrus_${testName} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/Core
        ${CMAKE_SOURCE_DIR}/src/Editor
        ${CMAKE_SOURCE_DIR}/src/FileBrowser
        ${CMAKE_SOURCE_DIR}/src/Rendering
        ${CMAKE_SOURCE_DIR}/src/Search
        ${CMAKE_SOURCE_DIR}/src/Syntax
        ${CMAKE_SOURCE_DIR}/src/Terminal
        ${CMAKE_SOURCE_DIR}/src/Views
    )

    target_link_libraries(papyrus_${testName} PRIVATE
        gtest_main
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
    )

    gtest_discover_tests(papyrus_${testName})
endfunction()

# Tests
add_simple_test(UndoManagerTests UndoManagerTests.cpp ../src/Editor/TextBuffer.cpp ../src/Editor/UndoManager.cpp)
add_simple_test(TextBufferTests TextBufferTests.cpp ../src/Editor/TextBuffer.cpp)
add_simple_test(TextInputTests TextInputTests.cpp ../src/Editor/TextInput.cpp)
add_simple_test(ArgParserTests ArgParserTests.cpp)
add_simple_test(LoggerTests LoggerTests.cpp)
add_simple_test(StartupParserTests StartupParserTests.cpp ../src/Core/StartupParser.cpp)
add_simple_test(EditorCommandHandlerTests EditorCommandHandlerTests.cpp ../src/Editor/EditorCommandHandler.cpp)
add_simple_test(ApplicationTests
    ApplicationTests.cpp
    ../src/Application.cpp
    ../src/Core/StartupParser.cpp
    ../src/Rendering/Renderer.cpp
    ../src/Rendering/SDLRenderBackend.cpp
    ../src/Rendering/CursorBlinker.cpp
    ../src/Views/EditorView.cpp
    ../src/Views/SearchView.cpp
    ../src/Views/TerminalView.cpp
    ../src/Views/FileBrowserView.cpp
    ../src/Views/EditorViewPort.cpp
    ../src/Views/SearchViewPort.cpp
    ../src/Views/TextLayout.cpp
    ../src/Views/LayoutManager.cpp
    ../src/Editor/Editor.cpp
    ../src/Editor/EditorCommandHandler.cpp
    ../src/Editor/UndoManager.cpp
    ../src/Editor/TextBuffer.cpp
    ../src/Editor/TextInput.cpp
    ../src/FileBrowser/FileBrowser.cpp
    ../src/Search/SearchSession.cpp
    ../src/Search/SearchEngine.cpp
    ../src/Syntax/SyntaxHighlighter.cpp
    ../src/Syntax/CppLexer.cpp
    ../src/Terminal/Terminal.cpp
    ../src/Terminal/CommandProcessor.cpp
)
