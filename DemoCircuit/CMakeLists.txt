set(EXEC_NAME "DS1-Demo")

target_include_directories("${EXEC_NAME}" PUBLIC ../CirSimLib)

add_executable("${EXEC_NAME}"
        ./DS1Run.cpp
        ./DS1Sim.cpp)

target_link_libraries("${EXEC_NAME}" PRIVATE CirSimLib)
