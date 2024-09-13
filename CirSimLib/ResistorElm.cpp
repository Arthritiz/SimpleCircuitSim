#include "ResistorElm.hpp"

ResistorElm::ResistorElm(CirSim* sim, double r, std::vector<int> nodes): CircuitElm(sim, std::move(nodes)) {
    if (r <= 0) {
        throw;
    }
    
    this->CircuitElm::baseAlloc();
    
    this->resistance = r;
}

void ResistorElm::stamp() {
    cirSim->stampResistor(this->nodes[0], this->nodes[1], this->resistance);
}
