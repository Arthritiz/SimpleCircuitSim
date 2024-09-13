#ifndef TransistorElm_hpp
#define TransistorElm_hpp

#include <cmath>
#include <vector>
#include <map>

#include "CircuitElm.hpp"

struct TransistorSpec {
    // [revise]: pnp to npn
    int pnp; // 1: npn, -1: pnp
    
    double satCur; // spice: IS
    
    double beta;
    double betaR;
    
    double BEleakCur;
    double BCleakCur;
    
    double leakBEemissionCoeff;
    double leakBCemissionCoeff;
    
    double emissionCoeffF;
    double emissionCoeffR;
    
    double invRollOffF;
    double invRollOffR;
    
    double invEarlyVoltF;
    double invEarlyVoltR;
};


class TransistorElm: public CircuitElm {
public:
    TransistorElm(CirSim*, std::string, std::vector<int>);
    
    int getPostCount() override { return 3; }
    void doStep() override;
    
    bool isNonLinear() override { return true; }
    
    constexpr static double VT = 0.025865;
    
private:
    double limitStep(double, double);
    
    TransistorSpec spec;
    
    double vcrit;
    
    double lastVbc;
    double lastVbe;
    
    double ib, ic, ie;
    
    const std::map<std::string, TransistorSpec> modelSpecs {
        {"default", TransistorSpec{1, 1e-13, 100.0, 1.0, 0.0, 0.0, 1.5, 2.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0}},
        {"spice-default", TransistorSpec{1, 1e-16, 100.0, 1.0, 0.0, 0.0, 1.5, 2.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0}}
    };
};
#endif /* TransistorElm_hpp */
