#ifndef DiodeElm_hpp
#define DiodeElm_hpp

#include <cmath>
#include <map>
#include "CircuitElm.hpp"

struct DiodeSpec {
    double leakage;
    double seriesResistance;
    double emissionCoefficient;
};

class DiodeElm: public CircuitElm {
public:
    DiodeElm(CirSim*, std::string, std::vector<int>);
    
    int getPostCount() override;
    void doStep() override;
    
    bool isNonLinear() override { return true; }
    
    constexpr static double vt = 0.025865;
    constexpr static double vzcoef = 1/DiodeElm::vt;
    
private:
    double limitStep(double, double);
    
    DiodeSpec spec;
    
    double vscale;
    double vdcoef;
    double vcrit;
    double vzcrit;
    
    int endNodeNum;
    bool hasResistor = false;
    double lastVoltdiff = 0.0;
    
    const std::map<std::string, DiodeSpec> modelSpecs {
        {"spice-default", DiodeSpec{1e-14, 0.0, 1.0}},
        {"default", DiodeSpec{1.7143528192808883e-7, 0.0, 2.0}}
    };
};

#endif /* DiodeElm_hpp */
