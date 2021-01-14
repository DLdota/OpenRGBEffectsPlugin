#ifndef RAINBOWWAVE_H
#define RAINBOWWAVE_H

#include "RGBEffect.h"
#include "OpenRGBEffectPage.h"

class RainbowWave: public RGBEffect
{
public:
    RainbowWave() {};
    ~RainbowWave() {};

    EffectInfo  DefineEffectDetails()                   override;
    void        StepEffect(std::vector<RGBController*>) override;

    EffectInfo EffectDetails;
};

#endif // RAINBOWWAVE_H
