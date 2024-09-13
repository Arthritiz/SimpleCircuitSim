#ifndef DS1Sim_hpp
#define DS1Sim_hpp

#include "Circuit.hpp"
#include "CirSim.hpp"
#include "CircuitElm.hpp"
#include "OpAmpElm.hpp"
#include "ResistorElm.hpp"
#include "CapacitorElm.hpp"
#include "TransistorElm.hpp"
#include "DiodeElm.hpp"

class DS1Sim: public Circuit {
public:
    DS1Sim(double, double);
    ~DS1Sim() override;
    
    void prepare(double) override;
    double process(double) override;
    
    void setDistortion(double);
    void setTone(double);
    
private:
    CirSim* bjtGainStage;
    CirSim* clipStage;
    CirSim* toneStage;
    
    const double DIST_R = 1e5;
    const double TONE_R = 20e3;
};

#endif /* DS1Sim_hpp */
