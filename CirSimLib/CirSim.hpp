#ifndef CirSim_hpp
#define CirSim_hpp

#include <iostream>
#include <Eigen/Dense>

#include "CircuitElm.hpp"

using namespace Eigen;

class CircuitElm;
class CirSim {
public:
    CirSim() = default;
    ~CirSim();
    
    void init(std::vector<CircuitElm*>);
    
    double getTimeStep() { return this->timeStep; }
    
    void prepare(double);
    void runOnce();
    void runOnce(double inputValue);
    
    void stampMatrix(int, int, double);
    void stampRightSide(int, double);
    
    void stampResistor(int, int, double);
    void stampConductance(int, int, double);
    void stampCurrentSource(int, int, double);
    void stampVoltageSource(int, int, int);
    void stampVoltageSource(int, int, int, double);
    void updateVoltageSource(int, double);
    void stampOpAmp(int, int, int, int, double);
    
    void setConverged(bool);
    
    void setCircuitVoltage(VectorXd&);
    double getNodeVoltage(int);
    
    void setVariableResistance(int, double);
    
private:
    void analyzeCircuit();
    void makeNodeLinks();
    void stampCircuit();
    void stampVariableResistors();
    
    // start from node 1
    MatrixXd circuitMatrix, origMatrix;
    VectorXd circuitRightSide, origRightSide, circuitSolve;
    
    double sampleRate = -1;
    double timeStep;
    double prevInputValue = 0;
    
    std::vector<CircuitElm*> elmArr;
    int inputElmIndex = -1;
    std::vector<std::vector<std::pair<int, int>>> circuitNodeLinks; // start from node 0 (ground)
    std::vector<int> vrIndices;
    
    int circuitNodeCount = 0; // from node 0
    int voltageSourceCount = 0;
    
    bool circuitNonLinear = false;
    bool converged;
};

#endif /* CirSim_hpp */
