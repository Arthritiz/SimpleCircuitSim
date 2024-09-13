cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "SimpleCiruitSim")
project("${PROJECT_NAME}" VERSION 0.0.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/CirSimLib
    ${CMAKE_SOURCE_DIR}/Ext/eigen
)

add_subdirectory(CirSimLib)
add_subdirectory(DemoCircuit)

