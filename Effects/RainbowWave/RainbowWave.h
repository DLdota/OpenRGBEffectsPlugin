#ifndef RAINBOWWAVE_H
#define RAINBOWWAVE_H

#include "RGBEffect.h"
#include "OpenRGBEffectPage.h"

class RainbowWave: public RGBEffect
{
public:
    RainbowWave() {};
    ~RainbowWave() {};

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QLayout*)                                override;
    void        StepEffect(std::vector<OwnedControllerAndZones> LocalControllers, int Step)  override;

    void        SetSpeed(int Speed)                                         override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override;
    void        ASelectionWasChanged()                                      override;

    void        SetWidth(int NewWidth);

    int                     GetSpeed()      override {return Speed; };
    int                     GetSlider2Val() override {return Width; };
    std::vector<RGBColor>   GetUserColors() override {return {};    };

    void                    EffectState(bool)override {return;};

    void                    LoadCustomSettings(json) override {return;};
    json                    SaveCustomSettings(json) override {return json();};

    EffectInfo EffectDetails;
private:
    int     Speed;
    int     Width;
    float   Progress = 0;
};

#endif // RAINBOWWAVE_H
