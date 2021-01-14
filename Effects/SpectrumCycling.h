#ifndef SPECTRUMCYCLING_H
#define SPECTRUMCYCLING_H

#include "RGBEffect.h"
#include "OpenRGBEffectPage.h"

class SpectrumCycling:
public RGBEffect
{
public:
    SpectrumCycling() {};
    ~SpectrumCycling() {};
    EffectInfo DefineEffectDetails()                   override;
    void       StepEffect(std::vector<RGBController*>) override;

    EffectInfo EffectDetails;
private:
    int CurrentProgress = 0;
};

#endif // SPECTRUMCYCLING_H
