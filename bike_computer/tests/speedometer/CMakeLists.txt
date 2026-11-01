cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(
    BikeComputerSpeedometerTests
    VERSION 0.1
    DESCRIPTION "CPP Zephyr Template application."
    LANGUAGES C CXX
)

target_sources(
    app
    PRIVATE
    src/test_speedometer.cpp
    ../../src/common/sensor_device.cpp
    ../../src/common/speedometer.cpp
)

include_directories(../../src)