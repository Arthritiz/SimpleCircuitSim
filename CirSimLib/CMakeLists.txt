set(LIB_NAME "CirSimLib")

set(SOURCES
        ${CMAKE_SOURCE_DIR}/CirSimLib/CapacitorElm.cpp
        ${CMAKE_SOURCE_DIR}/CirSimLib/CircuitElm.cpp
        ${CMAKE_SOURCE_DIR}/CirSimLib/CirSim.cpp
        ${CMAKE_SOURCE_DIR}/CirSimLib/DiodeElm.cpp
        ${CMAKE_SOURCE_DIR}/CirSimLib/OpAmpElm.cpp
        ${CMAKE_SOURCE_DIR}/CirSimLib/ResistorElm.cpp
        ${CMAKE_SOURCE_DIR}/CirSimLib/TransistorElm.cpp
)                               

add_library("${LIB_NAME}" STATIC ${SOURCES})
