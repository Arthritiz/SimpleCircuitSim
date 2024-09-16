#ifndef CircuitElm_hpp
#define CircuitElm_hpp

#include <vector>
#include "CirSim.hpp"

class CirSim;
class CircuitElm {
public:
    CircuitElm(CirSim*, std::vector<int>);
    virtual ~CircuitElm() = default;
    
    virtual void stamp() {}
    virtual void startIteration() {}
    virtual void doStep() {}
    virtual void finishIteration() {}
    virtual void resetState();
    void baseAlloc();
    
    virtual void setNodeVoltage(int n, double vol) { this->volts[n] = vol; }
    
    virtual bool isNonLinear() { return false; }
    std::vector<int>& getNodes() { return nodes; }
    virtual int getPostCount() { return 2; }
    
protected:
    CirSim* cirSim;
    std::vector<int> nodes;
    std::vector<double> volts;
};

class VoltageElm: public CircuitElm {
public:
    VoltageElm(CirSim*, std::vector<int>);
    VoltageElm(CirSim*, double, std::vector<int>);

    void stamp() override;
    void setVoltageSourceNumber(int n) { this->vsn = n; }

protected:
    int vsn = -1; // voltage source number in circuit context

private:
    double voltage;
};

class InputElm: public VoltageElm {
public:
    InputElm(CirSim*, std::vector<int>);

    void stamp() override;
    void doStep() override;
    void setInputValue(double);

private:
    double inputValue;
};

class VariableResistorElm: public CircuitElm {
public:
    VariableResistorElm(CirSim*, double, double, std::vector<int>);
    void stamp() override;
    bool stampUpdate();
    void setLRes(double r);
    int getPostCount() override;

private:
    double curLRes, targetLRes, totalRes;
};

#endif /* CircuitElm_hpp */
