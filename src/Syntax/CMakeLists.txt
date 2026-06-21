target_sources(papyrus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/SyntaxHighlighter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CppLexer.cpp
)

target_include_directories(papyrus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
