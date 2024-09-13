#include <iostream>
#include <cmath>
#include <chrono>

#include "Circuit.hpp"
#include "DS1Sim.hpp"

using namespace std::chrono;

int main(int argc, const char * argv[]) {
    double sampleRate = 44100.0;

    std::unique_ptr<Circuit> pedal = std::make_unique<DS1Sim>(0.5, 0.5);

    pedal->prepare(sampleRate);

    // input signal: sine wave
    double duration = 1.0; // unit: second
    double freq = 50;
    double amp = 1.0;

    int sampleCount = duration * sampleRate;
    std::vector<double> outputArr(sampleCount);
    
    double totalElapsed = 0.0;
    for (int i = 0; i < sampleCount; i++) {
        double t = i/sampleRate;
        auto start = high_resolution_clock::now();

        double inputValue = amp*sin(2*M_PI*freq*t); // generate input signal at one time step
        double outputValue = pedal->process(inputValue); // process the signal in pedal

        auto elapsed = duration_cast<microseconds>(high_resolution_clock::now() - start);

        totalElapsed += (double)elapsed.count()/1e6;
        
        outputArr.at(i) = outputValue;
    }

    std::cout << "total elapsed: " << totalElapsed << ", samples: " << sampleCount << "\n";
    std::cout << "average elapsed: " << totalElapsed/sampleCount << "\n";
    
    return 0;
}
