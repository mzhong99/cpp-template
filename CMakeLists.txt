cmake_minimum_required(VERSION 3.0)
project(verilator-project-template)

find_package(Threads REQUIRED)
find_package(verilator HINTS $ENV{VERILATOR_ROOT})

include_directories(external/doctest/doctest)
include_directories(external/nlohmann)
include_directories(external/spdlog/include)

file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(TEST_SRC
    test/DocTestMain.cpp
    test/TopModuleTest.cpp
    test/ArithCircuitTest.cpp
    test/DualPortROMTest.cpp)

set(VERILOG_SRC
    verilog/TopModule.v
    verilog/ArithCircuit.v
    verilog/DualPortROM.v)

add_executable(unittests ${TEST_SRC})
target_link_libraries(unittests Threads::Threads)
target_compile_definitions(unittests PRIVATE WAVEFORM_DIR="${WAVEFORM_DIR}")
verilate(unittests COVERAGE TRACE INCLUDE_DIRS verilog SOURCES verilog/TopModule.v )
verilate(unittests COVERAGE TRACE INCLUDE_DIRS verilog SOURCES verilog/ArithCircuit.v)
verilate(unittests COVERAGE TRACE INCLUDE_DIRS verilog SOURCES verilog/DualPortROM.v)

