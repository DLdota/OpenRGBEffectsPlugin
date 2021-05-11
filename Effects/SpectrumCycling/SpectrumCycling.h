#ifndef SPECTRUMCYCLING_H
#define SPECTRUMCYCLING_H

#include "RGBEffect.h"

class SpectrumCycling:
public RGBEffect
{
public:
    SpectrumCycling();
    ~SpectrumCycling() {};

    static std::string const ClassName() {return "SpectrumCycling";}

    void StepEffect(std::vector<ControllerZone>) override;

private:
    unsigned int CurrentHue = 0;
};

#endif // SPECTRUMCYCLING_H
