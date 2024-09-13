#ifndef OpAmpElm_hpp
#define OpAmpElm_hpp

#include <cmath>
#include "CircuitElm.hpp"

// ideal op amp with infinite apply voltage

class OpAmpElm: public VoltageElm {
public:
    OpAmpElm(CirSim*, std::vector<int>);

    int getPostCount() override { return 3; }

    void stamp() override;
    void doStep() override;

    bool isNonLinear() override { return true; }

private:
    double lastvd;
};

#endif /* OpAmpElm_hpp */
