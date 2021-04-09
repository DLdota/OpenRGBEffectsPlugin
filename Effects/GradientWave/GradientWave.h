#ifndef GRADIENTWAVE_H
#define GRADIENTWAVE_H

#include "RGBEffect.h"

class GradientWave: public RGBEffect
{
public:
    GradientWave() {};
    ~GradientWave() {};

    EffectInfo  DefineEffectDetails()                                                       override;
    void        DefineExtraOptions(QLayout*)                                                override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int)                       override;

    void        SetSpeed(int Speed)                                                         override;
    void        SetUserColors(std::vector<RGBColor>)                                        override;
    void        Slider2Changed(int)                                                         override;
    void        ToggleRandomColors(bool RandomEnabled)                                      override;

    void        SetWidth(int NewWidth);
    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>)                  override;

    int                     GetSpeed()                                                      override {return Speed;     };
    int                     GetSlider2Val()                                                 override {return 0;         };
    std::vector<RGBColor>   GetUserColors()                                                 override {return UserColors;};

    void                    EffectState(bool)                                               override {};

    void                    LoadCustomSettings(json)                                        override {return;};
    json                    SaveCustomSettings(json)                                        override {return json();};

    EffectInfo EffectDetails;
private:
    int                                         Speed;
    float                                       Width;

    std::vector<std::vector<float>>             Progress;
    std::vector<RGBColor>                       UserColors;

    bool     RandomColors = false;
    RGBColor RandomColorList[2];
};

#endif // GRADIENTWAVE_H
