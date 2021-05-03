#ifndef STARRYNIGHT_H
#define STARRYNIGHT_H

#include "RGBEffect.h"
#include <stdlib.h>
#include <hsv.h>

struct NewStar
{
    int         ControllerIndex;
    int         LED;
    float       state;
    RGBColor    Color;
};

class StarryNight: public RGBEffect
{
public:
    StarryNight() {};
    ~StarryNight() {};
    EffectInfo  DefineEffectDetails()                                       override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int)       override;
    void        Slider2Changed(int)                                         override;
    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>)  override;

    int         GetSlider2Val()                      override {return LEDPerCycle; };

    EffectInfo  EffectDetails;
private:
    int                         FPS;
    int                         TempLEDPerCycle = 1;
    int                         LEDPerCycle = 1;
    std::vector<NewStar>        CurrentStars;
};

#endif // STARRYNIGHT_H
