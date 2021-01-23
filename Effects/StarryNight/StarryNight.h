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
    EffectInfo  DefineEffectDetails() override;
    void        DefineExtraOptions(QWidget* Parent) override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int Step) override;

    void        SetSpeed(int Speed) override;
    void        SetUserColors(std::vector<RGBColor>) override;
    void        Slider2Changed(int) override;

    EffectInfo          EffectDetails;
private:
    int                                         Speed;
    int                                         FPS;
    int                                         LEDPerCycle = 1;
    std::vector<RGBColor>                       UserColors;
    //std::vector<OwnedControllerAndZones>        PreviousControllers;

    std::vector<NewStar>                           CurrentStars;
};

#endif // STARRYNIGHT_H
