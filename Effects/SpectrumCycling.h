#ifndef SPECTRUMCYCLING_H
#define SPECTRUMCYCLING_H

#include "RGBEffect.h"

class SpectrumCycling:
public RGBEffect
{
public:
    SpectrumCycling() {};
    ~SpectrumCycling() {};
    EffectInfo  DefineEffectDetails()       override;
    void        StartEffect()               override;
    void        StopEffect()                override;

    EffectInfo PluginDetails;
private:
    std::vector<RGBController*> OwnedDevices;
    bool StopNextLoop = false;
};

#endif // SPECTRUMCYCLING_H
