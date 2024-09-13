#ifndef ResistorElm_hpp
#define ResistorElm_hpp

#include "CircuitElm.hpp"

class ResistorElm: public CircuitElm {
public:
    ResistorElm(CirSim*, double, std::vector<int>);
    void stamp() override;

private:
    double resistance;
};

#endif /* ResistorElm_hpp */
