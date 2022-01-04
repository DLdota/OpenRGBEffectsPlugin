#ifndef RAINBOWWAVE_H
#define RAINBOWWAVE_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"

class RainbowWave: public RGBEffect
{
public:
    RainbowWave();
    ~RainbowWave() {};

    EFFECT_REGISTERER(ClassName(), CAT_RAINBOW, [](){return new RainbowWave;});

    static std::string const ClassName() { return "RainbowWave"; }

    void StepEffect(std::vector<ControllerZone*>) override;

private:
    float Progress = 0;
};

#endif // RAINBOWWAVE_H
