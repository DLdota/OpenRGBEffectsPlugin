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
    void        DefineExtraOptions(QLayout*)                                override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int Step)  override;

    void        SetSpeed(int Speed)                                         override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override;
    void        ASelectionWasChanged()                                      override;

    int                     GetSpeed()      override {return Speed;        };
    int                     GetSlider2Val() override {return LEDPerCycle;  };
    std::vector<RGBColor>   GetUserColors() override {return UserColors;   };

    void                    EffectStopping()override {return;};

    void                    LoadCustomSettings(json) override {return;};
    json                    SaveCustomSettings(json) override {return json();};

    EffectInfo          EffectDetails;
private:
    int                                         Speed;
    int                                         FPS;
    int                                         TempLEDPerCycle = 1;
    int                                         LEDPerCycle = 1;
    std::vector<RGBColor>                       UserColors;

    static std::vector<NewStar>                 CurrentStars;
};

#endif // STARRYNIGHT_H
