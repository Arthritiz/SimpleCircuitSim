#include "CircuitElm.hpp"

CircuitElm::CircuitElm(CirSim* sim, std::vector<int> nodes): cirSim(sim), nodes(nodes) {}

void CircuitElm::resetState() {
    std::fill(this->volts.begin(), this->volts.end(), 0);
}

void CircuitElm::baseAlloc() {
    auto elmNodeCount = this->getPostCount();
    
    if (nodes.size() != elmNodeCount) {
        throw;
    }
    
    for (auto n: nodes) {
        if (n < 0) throw;
    }
    
    this->volts.resize(elmNodeCount);
    std::fill(this->volts.begin(), this->volts.end(), 0);
}

// ===VoltageElm===

VoltageElm::VoltageElm(CirSim* sim, std::vector<int> nodes): CircuitElm(sim, std::move(nodes)) {}

VoltageElm::VoltageElm(CirSim* sim, double voltage, std::vector<int> nodes): CircuitElm(sim, std::move(nodes)) {
    if (voltage <= 0) {
        throw;
    }

    this->CircuitElm::baseAlloc();

    this->voltage = voltage;
}

void VoltageElm::stamp() {
    cirSim->stampVoltageSource(this->nodes[0], this->nodes[1], this->vsn, this->voltage);
}

// ===InputElm===

InputElm::InputElm(CirSim* sim, std::vector<int> nodes): VoltageElm(sim, std::move(nodes)) {
    this->CircuitElm::baseAlloc();
}

void InputElm::stamp() {
    cirSim->stampVoltageSource(this->nodes[0], this->nodes[1], this->vsn);
}

void InputElm::doStep() {
    cirSim->updateVoltageSource(this->vsn, this->inputValue);
}

void InputElm::setInputValue(double v) {
    this->inputValue = v;
}

// ===VariableResistorElm===

VariableResistorElm::VariableResistorElm(CirSim* sim, double r, std::vector<int> nodes): CircuitElm(sim, std::move(nodes)) {
    if (r <= 0) {
        throw;
    }
    
    this->CircuitElm::baseAlloc();
    
    this->currentResistance = r;
    this->targetResistance = r;
}

void VariableResistorElm::setResistance(double r) {
    if (r <= 0) {
        r = 1.0;
    }
    
    this->targetResistance = r;
}

void VariableResistorElm::stamp() {
    cirSim->stampResistor(this->nodes[0], this->nodes[1], this->currentResistance);
}

// if return true, it means stamp is called. if false, do nothing
bool VariableResistorElm::stampSmooth() { // [function not finish]
    if (this->currentResistance == this->targetResistance) {
        return false;
    }
    
    this->cirSim->stampConductance(this->nodes[0], this->nodes[1], 1/this->targetResistance - 1/this->currentResistance);
    
    this->currentResistance = this->targetResistance;
    
    return true;
}
