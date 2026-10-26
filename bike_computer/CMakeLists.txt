cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(
    bike_computer
    VERSION 0.1
    DESCRIPTION "BikeComputer application."
    LANGUAGES C CXX
)

FILE(GLOB app_sources src/*.cpp src/common/*.cpp src/common/resources/*.cpp src/static_scheduling/*.cpp)
target_sources(app PRIVATE ${app_sources})
target_include_directories(app PRIVATE src/)