cmake_minimum_required(VERSION 3.11)
project(lab3 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/src)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.12.1.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(lab3
        src/main.cpp
        src/marker.cpp
        src/array_utils.cpp
)

add_executable(runTests
        tests/array_utils_test.cpp
        src/array_utils.cpp
)
target_link_libraries(runTests gtest gtest_main)

add_test(NAME RunAllTests COMMAND runTests)
