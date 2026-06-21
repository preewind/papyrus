target_sources(papyrus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/EditorView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SearchView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TerminalView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FileBrowserView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EditorViewPort.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SearchViewPort.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TextLayout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LayoutManager.cpp
)

target_include_directories(papyrus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
