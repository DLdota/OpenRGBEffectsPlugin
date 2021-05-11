#ifndef RAINBOWWAVE_H
#define RAINBOWWAVE_H

#include "RGBEffect.h"
#include "OpenRGBEffectPage.h"

class RainbowWave: public RGBEffect
{
public:
    RainbowWave();
    ~RainbowWave() {};

    static std::string const ClassName() { return "RainbowWave"; }

    void StepEffect(std::vector<ControllerZone>) override;

private:
    float Progress = 0;
};

#endif // RAINBOWWAVE_H
