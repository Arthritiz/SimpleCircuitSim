#include "DiodeElm.hpp"

DiodeElm::DiodeElm(CirSim* sim, std::string modelName, std::vector<int> nodes): CircuitElm(sim, std::move(nodes)) {
    this->CircuitElm::baseAlloc();
    
    // get diode spec
    auto iter = DiodeElm::modelSpecs.find(modelName);
    if (iter == DiodeElm::modelSpecs.end()) {
        throw;
    }
    this->spec = iter->second;
    
    if (this->spec.seriesResistance > 0.0) {
        this->hasResistor = true;
    }
    
    this->vscale = this->spec.emissionCoefficient * DiodeElm::vt;
    this->vdcoef = 1/this->vscale;
    this->vcrit = this->vscale * std::log(this->vscale/(std::sqrt(2)*this->spec.leakage));
    
    this->vzcrit = DiodeElm::vt * std::log(DiodeElm::vt/(std::sqrt(2)*this->spec.leakage));
    
    if (this->hasResistor) {
        this->endNodeNum = 2;
    } else {
        this->endNodeNum = 1;
    }
}

int DiodeElm::getPostCount() {
    if (this->hasResistor) return 3;
    else return 2;
}

double DiodeElm::limitStep(double vnew, double vold) {
    double arg;
    
    if ((vnew > this->vcrit) && (abs(vnew - vold) > (this->vscale + this->vscale))) {
        if (vold > 0) {
            arg = 1 + (vnew - vold)/this->vscale;
            if (arg > 0) {
                vnew = vold + this->vscale * std::log(arg);
            } else {
                vnew = this->vcrit;
            }
        } else {
            vnew = this->vscale * std::log(vnew/this->vscale);
        }
        
        this->cirSim->setConverged(false);
    }
    
    return vnew;
}

void DiodeElm::doStep() {
    double voltdiff = this->volts[0] - this->volts[this->endNodeNum];
    
    if (abs(voltdiff - this->lastVoltdiff) > 0.01) {
        this->cirSim->setConverged(false);
    }
    
    voltdiff = this->limitStep(voltdiff, this->lastVoltdiff);
    this->lastVoltdiff = voltdiff;
    
    double gmin = this->spec.leakage * 0.01;
    
    double eval = std::pow(std::exp(1.0), voltdiff*this->vdcoef);
    double geq = this->vdcoef * this->spec.leakage * eval + gmin;
    double nc = (eval - 1)*this->spec.leakage - geq * voltdiff;
    
    this->cirSim->stampConductance(this->nodes[0], this->nodes[1], geq);
    this->cirSim->stampCurrentSource(this->nodes[0], this->nodes[1], nc);
}
