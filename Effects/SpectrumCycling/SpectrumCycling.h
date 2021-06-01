#ifndef SPECTRUMCYCLING_H
#define SPECTRUMCYCLING_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"

class SpectrumCycling:
public RGBEffect
{
public:
    SpectrumCycling();
    ~SpectrumCycling() {};

    EFFECT_REGISTERER(ClassName(), [](){return new SpectrumCycling;});

    static std::string const ClassName() {return "SpectrumCycling";}

    void StepEffect(std::vector<ControllerZone>) override;

private:
    float CurrentHue = 0;
};

#endif // SPECTRUMCYCLING_H
