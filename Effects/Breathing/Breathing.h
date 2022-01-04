#ifndef BREATHING_H
#define BREATHING_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"

class Breathing: public RGBEffect
{
public:
    Breathing();
    ~Breathing(){}

    EFFECT_REGISTERER(ClassName(), CAT_SIMPLE, [](){return new Breathing;});

    static std::string const ClassName() { return "Breathing"; }

    void StepEffect(std::vector<ControllerZone*>) override;
    void SetUserColors(std::vector<RGBColor>) override;

private:
    float Progress = 0;
    hsv_t CurrentColor;
};

#endif // BREATHING_H
