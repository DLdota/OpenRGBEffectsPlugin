#ifndef STARRYNIGHT_H
#define STARRYNIGHT_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"

struct NewStar
{
    unsigned int        Index;
    unsigned int        LED;
    float               state;
    RGBColor            Color;
};

class StarryNight: public RGBEffect
{
public:
    StarryNight();
    ~StarryNight() {};

    EFFECT_REGISTERER(ClassName(), [](){return new StarryNight;});

    static std::string const ClassName() {return "StarryNight";}

    void StepEffect(std::vector<ControllerZone*>) override;
    void OnControllerZonesListChanged(std::vector<ControllerZone*>) override;

private:
    std::vector<NewStar> CurrentStars;
};

#endif // STARRYNIGHT_H
