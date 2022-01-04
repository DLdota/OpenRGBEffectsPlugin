#ifndef GRADIENTWAVE_H
#define GRADIENTWAVE_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"

class GradientWave: public RGBEffect
{
public:
    GradientWave();
    ~GradientWave(){}

    EFFECT_REGISTERER(ClassName(), CAT_ADVANCED, [](){return new GradientWave;});

    static std::string const ClassName() { return "GradientWave"; }

    void StepEffect(std::vector<ControllerZone*>) override;
    void SetRandomColorsEnabled(bool) override;
    void OnControllerZonesListChanged(std::vector<ControllerZone*>) override;

private:
    std::vector<float> Progress;
    RGBColor RandomColorList[2];
};

#endif // GRADIENTWAVE_H
