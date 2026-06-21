target_sources(papyrus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SDLRenderBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CursorBlinker.cpp
)

target_include_directories(papyrus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
