cmake_minimum_required(VERSION 3.0)
project(verilator-project-template)

find_package(Threads REQUIRED)

include_directories(external/doctest/doctest)
include_directories(external/nlohmann)
include_directories(external/spdlog/include)

set(CMAKE_CXX_STANDARD 20)
set(TEST_SRC
    test/DocTestMain.cpp)

add_executable(unittests ${TEST_SRC})
target_link_libraries(unittests Threads::Threads)

