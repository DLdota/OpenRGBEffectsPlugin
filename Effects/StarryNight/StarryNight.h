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
    void        ASelectionWasChanged() override;

    int                     GetSpeed()      override {return Speed;        };
    int                     GetSlider2Val() override {return LEDPerCycle;  };
    std::vector<RGBColor>   GetUserColors() override {return UserColors;   };

    EffectInfo          EffectDetails;
private:
    int                                         Speed;
    int                                         FPS;
    int                                         LEDPerCycle = 1;
    std::vector<RGBColor>                       UserColors;
    //std::vector<OwnedControllerAndZones>        PreviousControllers;

    static std::vector<NewStar>                 CurrentStars;
};

#endif // STARRYNIGHT_H
