#ifndef GRADIENTCYCLING_H
#define GRADIENTCYCLING_H

#include "RGBEffect.h"

class GradientCycling: public RGBEffect
{
public:
    GradientCycling() {};
    ~GradientCycling() {};

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QLayout*)                                override;
    void        StepEffect(std::vector<OwnedControllerAndZones> LocalControllers, int Step)  override;

    void        SetSpeed(int Speed)                                         override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override;

    void        SetWidth(int NewWidth);
    void        ASelectionWasChanged() override;

    int                     GetSpeed()       override {return Speed;     };
    int                     GetSlider2Val()  override {return 0;         };
    std::vector<RGBColor>   GetUserColors()  override {return UserColors;};

    void                    EffectStopping() override {return;};

    void                    LoadCustomSettings(json) override {return;};
    json                    SaveCustomSettings(json) override {return json();};

    EffectInfo EffectDetails;
private:
    static std::vector<OwnedControllerAndZones> PrevControllers;

    int                     Speed;
    float                   Width;

    std::vector<std::vector<float>> Progress;
    std::vector<RGBColor>           UserColors;


};

#endif // GRADIENTCYCLING_H
