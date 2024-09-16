set(LIB_NAME "CirSimLib")

set(SOURCES
        ./CapacitorElm.cpp
        ./CircuitElm.cpp
        ./CirSim.cpp
        ./DiodeElm.cpp
        ./OpAmpElm.cpp
        ./ResistorElm.cpp
        ./TransistorElm.cpp
)

include_directories(../Ext/eigen)

add_library("${LIB_NAME}" STATIC ${SOURCES})
