target_sources(papyrus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Terminal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CommandProcessor.cpp
)

target_include_directories(papyrus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
