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
    EffectInfo DefineEffectDetails()                             override;
    void       StepEffect(std::vector<OwnedControllerAndZones>, int Step) override;

    void        SetSpeed(int Speed)                              override;
    void        SetUserColors(std::vector<RGBColor>)             override;

    EffectInfo EffectDetails;
private:
    int CurrentHue = 0;
    int Speed;
};

#endif // SPECTRUMCYCLING_H
