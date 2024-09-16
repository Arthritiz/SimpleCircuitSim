#include "CircuitElm.hpp"

inline bool doublesEqual(double a, double b, double epsilon = 1e-9) {
    return abs(a - b) < epsilon;
}

CircuitElm::CircuitElm(CirSim* sim, std::vector<int> nodes): cirSim(sim), nodes(nodes) {}

void CircuitElm::resetState() {
    std::fill(this->volts.begin(), this->volts.end(), 0);
}

void CircuitElm::baseAlloc() {
    auto elmNodeCount = this->getPostCount();
    
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

VariableResistorElm::VariableResistorElm(CirSim* sim, double lRes, double totalRes,std::vector<int> nodes): CircuitElm(sim, std::move(nodes)) {
    if (lRes < 0 || totalRes <= 0 || lRes > totalRes) {
        throw;
    }

    if (this->nodes.size() != 3)
    {
        throw;
    }

    this->CircuitElm::baseAlloc();
    
    if (lRes <= 0)
    {
        lRes = 1.0;
    }

    if (doublesEqual(lRes, totalRes))
    {
        lRes = totalRes - 1.0;
    }

    this->curLRes = lRes;
    this->targetLRes = lRes;
    this->totalRes = totalRes;
}

void VariableResistorElm::setLRes(double lRes) {
    if (lRes <= 0) {
        lRes = 1.0;
    }

    if (doublesEqual(lRes, this->totalRes))
    {
        lRes = this->totalRes - 1.0;
    }
    
    this->targetLRes = lRes;
}

int VariableResistorElm::getPostCount()
{
    if ((nodes[0] == nodes[1]) || (nodes[1] == nodes[2]))
    {
        return 2;
    } else
    {
        return 3;
    }
}

void VariableResistorElm::stamp() {
    if (this->nodes[0] != this->nodes[1])
    {
        cirSim->stampResistor(this->nodes[0], this->nodes[1], this->curLRes);
    }

    if (this->nodes[1] != this->nodes[2])
    {
        cirSim->stampResistor(this->nodes[1], this->nodes[2], this->totalRes - this->curLRes);
    }
}

// if return true, it means stamp is called. if false, do nothing
bool VariableResistorElm::stampUpdate() {
    if (this->curLRes == this->targetLRes) {
        return false;
    }

    if (this->nodes[0] != this->nodes[1])
    {
        this->cirSim->stampConductance(this->nodes[0], this->nodes[1], 1/this->targetLRes - 1/this->curLRes);
    }

    if (this->nodes[1] != this->nodes[2])
    {
        double targetRRes = this->totalRes - this->targetLRes;
        double curRRes = this->totalRes - this->curLRes;

        this->cirSim->stampConductance(this->nodes[1], this->nodes[2], 1/targetRRes - 1/curRRes);
    }

    this->curLRes = this->targetLRes;

    return true;
}
