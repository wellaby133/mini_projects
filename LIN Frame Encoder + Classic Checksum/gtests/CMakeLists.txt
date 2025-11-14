cmake_minimum_required(VERSION 3.10)

# Google Test
enable_testing()

# Fetch Google Test from GitHub
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(test_main 
    test_main.cpp
    ../code/lin_checksum.c
)

target_include_directories(test_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../code)

target_link_libraries(test_main 
    gtest 
    gtest_main
)

add_test(NAME LinChecksumTests COMMAND test_main)
