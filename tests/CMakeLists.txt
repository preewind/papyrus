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
add_simple_test(UtilTests UtilTests.cpp)
add_simple_test(ThemeTests ThemeTests.cpp)
add_simple_test(TypesTests TypesTests.cpp)
add_simple_test(StartupParserTests StartupParserTests.cpp ../src/Core/StartupParser.cpp)
add_simple_test(EditorCommandHandlerTests EditorCommandHandlerTests.cpp ../src/Editor/EditorCommandHandler.cpp)
add_simple_test(SearchTests SearchTests.cpp ../src/Search/SearchEngine.cpp ../src/Search/SearchSession.cpp ../src/Editor/TextInput.cpp ../src/Editor/TextBuffer.cpp)
add_simple_test(FileBrowserTests FileBrowserTests.cpp ../src/FileBrowser/FileBrowser.cpp)
add_simple_test(TerminalTests TerminalTests.cpp ../src/Terminal/Terminal.cpp ../src/Terminal/CommandProcessor.cpp ../src/Editor/TextInput.cpp ../src/Editor/TextBuffer.cpp)
add_simple_test(SyntaxTests SyntaxTests.cpp ../src/Syntax/CppLexer.cpp ../src/Syntax/SyntaxHighlighter.cpp ../src/Editor/TextBuffer.cpp)
