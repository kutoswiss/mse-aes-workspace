cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(
    BikeComputer
    VERSION 0.1
    DESCRIPTION "CPP Zephyr Template application."
    LANGUAGES C CXX
)

target_sources(
    app
    PRIVATE
    src/main.cpp
    src/common/bike_display.cpp
    src/common/sensor_device.cpp
    src/common/speedometer.cpp
    src/common/task_manager.cpp
    src/static_scheduling/gear_device.cpp
)

target_include_directories(
    app
    PRIVATE
    src
    src/common
)