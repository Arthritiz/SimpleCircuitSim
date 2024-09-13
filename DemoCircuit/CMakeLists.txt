set(EXEC_NAME "DS1-Demo")

include_directories(
    ${CMAKE_SOURCE_DIR}/DemoCircuit
)

add_executable("${EXEC_NAME}"
        ${CMAKE_SOURCE_DIR}/DemoCircuit/DS1Run.cpp
        ${CMAKE_SOURCE_DIR}/DemoCircuit/DS1Sim.cpp)

target_link_libraries("${EXEC_NAME}" PRIVATE CirSimLib)
