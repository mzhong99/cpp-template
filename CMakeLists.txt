cmake_minimum_required(VERSION 3.0)
project(cpp-project-template)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

include_directories(external/doctest/doctest)
include_directories(external/nlohmann)
include_directories(external/spdlog/include)

include_directories(src)
include_directories(src/utils)

set(COMMON_SRC
    src/utils/SoftwareBusSubsystem.cpp)

set(TEST_SRC
    test/MessageQueueTest.cpp
    test/ListTest.cpp
    test/TrieTest.cpp
    test/SubsystemTest.cpp
    test/SoftwareBusSubsystemTest.cpp
    test/DocTestMain.cpp)

add_executable(unittests ${TEST_SRC} ${COMMON_SRC})
target_link_libraries(unittests Threads::Threads)

