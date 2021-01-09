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
    EffectInfo  DefineEffectDetails()                   override;
    void        StartEffect()                           override;
    void        StopEffect()                            override;

    void        SetSpeed(int Speed)                     override;
    void        SetUserColors(std::vector<UserColor>)   override;

    EffectInfo EffectDetails;
private:
    std::vector<RGBController*> OwnedDevices;
    bool StopNextLoop = false;
    int Speed;
};

#endif // SPECTRUMCYCLING_H
