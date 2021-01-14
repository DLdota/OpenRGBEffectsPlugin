#ifndef RAINBOWWAVE_H
#define RAINBOWWAVE_H

#include "RGBEffect.h"
#include "OpenRGBEffectPage.h"

class RainbowWave: public RGBEffect
{
public:
    RainbowWave() {};
    ~RainbowWave() {};

    EffectInfo  DefineEffectDetails()                               override;
    void        StepEffect(std::vector<RGBController*>, int Step)   override;

    void        SetSpeed(int Speed)                                 override;
    void        SetUserColors(std::vector<RGBColor>)                override;

    EffectInfo EffectDetails;
private:
    int Speed;
};

#endif // RAINBOWWAVE_H
