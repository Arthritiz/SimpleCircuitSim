#include "CirSim.hpp"

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

void CirSim::init(std::vector<CircuitElm*> elmArr) {
    this->elmArr = std::move(elmArr);
    
    this->analyzeCircuit();
    this->makeNodeLinks();
}

CirSim::~CirSim() {
    for (auto* elm: this->elmArr) {
        delete elm;
    }
}

void CirSim::prepare(double sampleRate) {
    if (sampleRate == this->sampleRate) return;
    
    this->sampleRate = sampleRate;
    this->timeStep = 1/sampleRate;
    
    for (auto* elm: this->elmArr)
        elm->resetState();
    
    this->stampCircuit();
}

void CirSim::analyzeCircuit() {
    int maxNodeNumber = -1;
    int nodeNumberLimit = -1;
    int inputElmCount = 0;
    
    // illegal check
    for (auto* elm: this->elmArr) {
        auto& elmNodes = elm->getNodes();
        
        for (auto n: elmNodes) maxNodeNumber = std::max(maxNodeNumber, n);
        
         nodeNumberLimit += elm->getPostCount() - 1;
        
        if (instanceof<InputElm>(elm)) {
            inputElmCount += 1;
        }
    }
            
    // only one InputElm can exist in circuit
    if (inputElmCount > 1) {
        throw;
    }
    
    if (maxNodeNumber > nodeNumberLimit) {
        throw;
    }
    
    this->circuitNodeCount = maxNodeNumber + 1; // add node 0 (ground)
        
    for (int i = 0; i < this->elmArr.size(); i++) {
        auto* elm = this->elmArr[i];
        
        // VoltageElm detect
        if (instanceof<VoltageElm>(elm)) {
            static_cast<VoltageElm*>(elm)->setVoltageSourceNumber(this->voltageSourceCount++);
        }
        
        // InputElm detect
        if (instanceof<InputElm>(elm)) {
            inputElmIndex = i;
        }
        
        // VariableResistor detect
        if (instanceof<VariableResistorElm>(elm)) {
            this->vrIndices.push_back(i);
        }
    
        // Non-Linear detect
        if (elm->isNonLinear() && !this->circuitNonLinear) {
            this->circuitNonLinear = true;
        }
        
    }
}

void CirSim::makeNodeLinks() {
    this->circuitNodeLinks.resize(this->circuitNodeCount);
    
    for (int i = 0; i < this->elmArr.size(); i++) {
        auto* elm = this->elmArr[i];
        auto& elmNodes = elm->getNodes();
        
        for (int eNumLocal = 0; eNumLocal < elmNodes.size(); eNumLocal++) {
            auto eNumCircuit = elmNodes[eNumLocal];
            
            // pair means <index of elm, local node number of elm>
            circuitNodeLinks[eNumCircuit].emplace_back(i, eNumLocal);
        }
    }
}
    
void CirSim::stampCircuit() {
    // update size of circuitMatrix and circuitRightSide
    int matrixSize = this->circuitNodeCount - 1 + this->voltageSourceCount; // from node 1
    circuitMatrix.resize(matrixSize, matrixSize);
    circuitRightSide.resize(matrixSize);
    
    circuitMatrix.setZero();
    circuitRightSide.setZero();
    
    // stamp all elm
    for (auto* elm: this->elmArr)
        elm->stamp();
    
    this->origMatrix = this->circuitMatrix;
    this->origRightSide = this->circuitRightSide;
}

void CirSim::setVarLRes(int ri, double r) {
    if (ri >= this->vrIndices.size()) {
        return;
    }
    
    auto i = this->vrIndices[ri];
    static_cast<VariableResistorElm*>(this->elmArr[i])->setLRes(r);
}

void CirSim::stampVariableResistors() {
    bool change = false;
    
    this->circuitMatrix = this->origMatrix;
    
    for (auto i: this->vrIndices) {
        bool isStamp = static_cast<VariableResistorElm*>(this->elmArr[i])->stampUpdate();
        
        if (isStamp && !change) {
            change = true;
        }
    }
    
    if (change) {
        this->origMatrix = this->circuitMatrix;
    }
}

void CirSim::stampMatrix(int i, int j, double val) {
    if (i > 0 && j > 0) {
        i--;
        j--;
        
        this->circuitMatrix(i, j) += val;
    }
}

void CirSim::stampRightSide(int i, double val) {
    if (i > 0) {
        i--;
        
        this->circuitRightSide(i) += val;
    }
}

void CirSim::stampResistor(int n1, int n2, double r) {
    double g = 1/r;
    
    this->stampMatrix(n1, n1, g);
    this->stampMatrix(n2, n2, g);
    this->stampMatrix(n1, n2, -g);
    this->stampMatrix(n2, n1, -g);
}

void CirSim::stampConductance(int n1, int n2, double g) {
    this->stampMatrix(n1, n1, g);
    this->stampMatrix(n2, n2, g);
    this->stampMatrix(n1, n2, -g);
    this->stampMatrix(n2, n1, -g);
}

void CirSim::stampCurrentSource(int n1, int n2, double val) {
    this->stampRightSide(n1, -val);
    this->stampRightSide(n2, val);
}

void CirSim::stampVoltageSource(int n1, int n2, int vsn) {
    int vn = this->circuitNodeCount + vsn;
    
    this->stampMatrix(vn, n1, -1);
    this->stampMatrix(vn, n2, 1);
    this->stampMatrix(n1, vn, 1);
    this->stampMatrix(n2, vn, -1);
}

void CirSim::stampVoltageSource(int n1, int n2, int vsn, double val) {
    int vn = this->circuitNodeCount + vsn;
    
    this->stampMatrix(vn, n1, -1);
    this->stampMatrix(vn, n2, 1);
    this->stampMatrix(n1, vn, 1);
    this->stampMatrix(n2, vn, -1);
    
    this->stampRightSide(vn, val);
}
    
void CirSim::updateVoltageSource(int vsn, double val) {
    int vn = this->circuitNodeCount + vsn;
    
    this->stampRightSide(vn, val);
}

void CirSim::stampOpAmp(int n1, int n2, int n3, int vsn, double gain) {
    int vn = this->circuitNodeCount + vsn;
    
    this->stampMatrix(n3, vn, 1);
    
    this->stampMatrix(vn, n1, gain);
    this->stampMatrix(vn, n2, -gain);
    this->stampMatrix(vn, n3, 1);
}


void CirSim::setConverged(bool b) {
    this->converged = b;
}

double CirSim::getNodeVoltage(int n) {
    if (this->circuitSolve.size() < n) {
        throw;
    }
    
    return this->circuitSolve[n-1];
}

void CirSim::setCircuitVoltage(VectorXd& solved) {
    for (int i = 1; i < this->circuitNodeCount; i++) { // from node 1
        for (auto& pairs: this->circuitNodeLinks[i]) {
            this->elmArr[pairs.first]->setNodeVoltage(pairs.second, solved[i-1]);
        }
    }
    
    this->circuitSolve = solved;
}

void CirSim::runOnce() {
    this->stampVariableResistors();
    
    for (auto* elm: this->elmArr) {
        elm->startIteration();
    }
    
    int subIter, subIterCount = 100;
    for (subIter = 0; subIter < subIterCount; subIter++) {
        this->setConverged(true);
        
        this->circuitMatrix = this->origMatrix;
        this->circuitRightSide = this->origRightSide;
        
        for (auto* elm: this->elmArr) {
            elm->doStep();
        }
        
        if (this->circuitNonLinear) {
            if (subIter > 0 && this->converged) {
                break;
            }
        }
        
        VectorXd solved = this->circuitMatrix.lu().solve(this->circuitRightSide);
        
        this->setCircuitVoltage(solved);
    
        if (!this->circuitNonLinear) break;
    }
    
    if (subIter == subIterCount) {
        std::cout << "converge failed" << std::endl;
    }
    
    for (auto* elm: this->elmArr) {
        elm->finishIteration();
    }
}

void CirSim::runOnce(double inputValue) {
    if (this->elmArr.size() <= 0) {
        return;
    }
    
    if (inputElmIndex >= 0) {
        static_cast<InputElm*>(this->elmArr[this->inputElmIndex])->setInputValue(inputValue);
    }
    
    runOnce();
    
    this->prevInputValue = inputValue;
}
