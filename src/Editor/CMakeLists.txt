target_sources(papyrus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UndoManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TextBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TextInput.cpp
)

target_include_directories(papyrus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
