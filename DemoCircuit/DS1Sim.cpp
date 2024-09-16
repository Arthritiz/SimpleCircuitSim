#include "DS1Sim.hpp"

DS1Sim::DS1Sim(double distortionRatio, double toneRatio) {
    this->bjtGainStage = new CirSim();
    this->bjtGainStage->init(std::vector<CircuitElm*> {
                                    new InputElm(bjtGainStage, std::vector<int>{0, 1}),
                                    new CapacitorElm(bjtGainStage, 47e-9, std::vector<int>{1, 2}),
                                    new ResistorElm(bjtGainStage, 1e5, std::vector<int>{2, 0}),
                                    new TransistorElm(bjtGainStage, "spice-default", std::vector<int>{2, 3, 4}),
                                    new CapacitorElm(bjtGainStage, 250e-12, std::vector<int>{2, 3}),
                                    new ResistorElm(bjtGainStage, 4.7e5, std::vector<int>{2, 3}),
                                    new ResistorElm(bjtGainStage, 22, std::vector<int>{4, 0}),
                                    new ResistorElm(bjtGainStage, 10e3, std::vector<int>{3, 5}),
                                    new VoltageElm(bjtGainStage, 9.0, std::vector<int>{0, 5})
    });
    
    this->clipStage = new CirSim();
    this->clipStage->init(std::vector<CircuitElm*> {
                                            new CapacitorElm(clipStage, 68e-9, std::vector<int>{1, 2}),
                                            new ResistorElm(clipStage, 1e5, std::vector<int>{2, 0}),
                                            new OpAmpElm(clipStage, std::vector<int>{3, 2, 4}),
                                            new ResistorElm(clipStage, 1e5, std::vector<int>{3, 5}),
                                            new CapacitorElm(clipStage, 100e-12, std::vector<int>{5, 4}),
                                            new VariableResistorElm(clipStage, distortionRatio*this->DIST_R, this->DIST_R, std::vector<int>{4, 5, 6}),
                                            new ResistorElm(clipStage, 4.7e3, std::vector<int>{6, 9}),
                                            new CapacitorElm(clipStage, 470e-9, std::vector<int>{9, 0}),
                                            new ResistorElm(clipStage, 2.2e3, std::vector<int>{4, 7}),
                                            new CapacitorElm(clipStage, 470e-9, std::vector<int>{7, 8}),
                                            new CapacitorElm(clipStage, 10e-9, std::vector<int>{8, 0}),
                                            new DiodeElm(clipStage, "spice-default", std::vector<int>{8, 0}),
                                            new DiodeElm(clipStage, "spice-default", std::vector<int>{0, 8}),
                                            new InputElm(clipStage, std::vector<int>{0, 1}),
    });
    
    this->toneStage = new CirSim();
    this->toneStage->init(std::vector<CircuitElm*> {
                                            new InputElm(toneStage, std::vector<int>{0, 1}),
                                            new CapacitorElm(toneStage, 22e-9, std::vector<int>{1, 2}),
                                            new ResistorElm(toneStage, 2.2e3, std::vector<int>{2, 3}),
                                            new ResistorElm(toneStage, 6.8e3, std::vector<int>{3, 0}),
                                            new ResistorElm(toneStage, 6.8e3, std::vector<int>{1, 5}),
                                            new CapacitorElm(toneStage, 0.1e-6, std::vector<int>{5, 0}),
                                            new VariableResistorElm(toneStage, toneRatio*this->TONE_R, this->TONE_R, std::vector<int>{5, 4, 3})
    });
}

DS1Sim::~DS1Sim() {
    delete this->bjtGainStage;
    delete this->clipStage;
    delete this->toneStage;
}

void DS1Sim::setDistortion(double ratio) {
    if (ratio < 0.0) {
        ratio = 0.0;
    } else if (ratio > 1.0) {
        ratio = 1.0;
    }
    
    this->clipStage->setVarLRes(0, ratio*this->DIST_R);
    this->clipStage->setVarLRes(1, (1-ratio)*this->DIST_R + 4.7e3);
}

void DS1Sim::setTone(double ratio) {
    if (ratio < 0.0) {
        ratio = 0.0;
    } else if (ratio > 1.0) {
        ratio = 1.0;
    }
    
    this->toneStage->setVarLRes(0, ratio*this->TONE_R);
    this->toneStage->setVarLRes(1, (1-ratio)*TONE_R);
}

void DS1Sim::prepare(double sampleRate) {
    this->bjtGainStage->prepare(sampleRate);
    this->clipStage->prepare(sampleRate);
    this->toneStage->prepare(sampleRate);
}

double DS1Sim::process(double inputValue) {
    this->bjtGainStage->runOnce(inputValue);

    double outputValue = this->bjtGainStage->getNodeVoltage(3);
    this->clipStage->runOnce(outputValue);
    
    outputValue = this->clipStage->getNodeVoltage(8);
    
    this->toneStage->runOnce(outputValue);

    return this->toneStage->getNodeVoltage(4);
}
