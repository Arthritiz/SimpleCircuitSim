#ifndef CapacitorElm_hpp
#define CapacitorElm_hpp

#include "CircuitElm.hpp"

class CapacitorElm: public CircuitElm {
public:
    CapacitorElm(CirSim*, double, std::vector<int>);
    
    void resetState();
    void stamp();
    void startIteration();
    void doStep();
    void finishIteration();

private:
    bool isTrapezoidal() { return false; }
    
    double capacitance;
    double compResistance;
    
    double current;
    double iseq; // equivalent current source
};

#endif /* CapacitorElm_hpp */
