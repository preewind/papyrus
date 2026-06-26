target_sources(papyrus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Screensaver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DvdScreensaver.cpp
)

target_include_directories(papyrus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})