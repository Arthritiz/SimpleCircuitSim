#include "CapacitorElm.hpp"

CapacitorElm::CapacitorElm(CirSim* sim, double cap, std::vector<int> nodes): CircuitElm(sim, std::move(nodes)) {
    if (cap <= 0) {
        throw;
    }
    
    this->CircuitElm::baseAlloc();
    
    this->capacitance = cap;
    this->current = 0.0;
}

void CapacitorElm::resetState() {
    std::fill(this->volts.begin(), this->volts.end(), 0);
    
    this->current = 0.0;
}

void CapacitorElm::stamp() {
    if (isTrapezoidal()) {
        this->compResistance = this->cirSim->getTimeStep()/(2 * this->capacitance); // Implicit Trapezoidal Rule
    } else {
        this->compResistance = this->cirSim->getTimeStep()/(1 * this->capacitance); // Backward Euler
    }
    
    cirSim->stampResistor(this->nodes[0], this->nodes[1], this->compResistance);
}

void CapacitorElm::startIteration() {
    double voltdiff = this->volts[0] - this->volts[1];
    
    if (isTrapezoidal()) {
        this->iseq = -voltdiff/this->compResistance - current;
    } else {
        this->iseq = -voltdiff/this->compResistance;
    }
}

void CapacitorElm::doStep() {
    this->cirSim->stampCurrentSource(this->nodes[0], this->nodes[1], this->iseq);
}

void CapacitorElm::finishIteration() {
    double voltdiff = this->volts[0] - this->volts[1];
    
    this->current = voltdiff/this->compResistance + this->iseq;
}