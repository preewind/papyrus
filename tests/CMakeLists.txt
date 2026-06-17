list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/third_party/googletest/cmake")
include(GoogleTest)

function(add_simple_test testName src)
    add_executable(papyrus_${testName}
        ${src}
        ${ARGN}
    )

    target_include_directories(papyrus_${testName} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

    target_link_libraries(papyrus_${testName} PRIVATE
        gtest_main
        SDL3::SDL3
    )

    gtest_discover_tests(papyrus_${testName})
endfunction()

# Tests
add_simple_test(UndoManagerTests UndoManagerTests.cpp ../src/TextBuffer.cpp ../src/UndoManager.cpp)
add_simple_test(TextBufferTests TextBufferTests.cpp ../src/TextBuffer.cpp)
