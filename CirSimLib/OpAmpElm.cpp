#include "OpAmpElm.hpp"

OpAmpElm::OpAmpElm(CirSim* sim, std::vector<int> nodes): VoltageElm(sim, std::move(nodes)) {
    this->CircuitElm::baseAlloc();
}

void OpAmpElm::stamp() {
    this->cirSim->stampOpAmp(this->nodes[0], this->nodes[1], this->nodes[2], this->vsn, 1e5);
}

void OpAmpElm::doStep() {
    double vd = this->volts[1] - this->volts[0];

    if(abs(this->lastvd - vd) > 0.1) {
        this->cirSim->setConverged(false);
    }

    this->lastvd = vd;
}