cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(
    BikeComputerTests
    VERSION 0.1
    DESCRIPTION "CPP Zephyr Template application."
    LANGUAGES C CXX
)

target_sources(
    app
    PRIVATE
    src/main.cpp
    ../../src/common/constants.hpp
    ../../src/common/sensor_device.cpp
    ../../src/common/sensor_device.hpp
)

include_directories(../../src)