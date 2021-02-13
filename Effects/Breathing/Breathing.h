#ifndef BREATHING_H
#define BREATHING_H

#include "RGBEffect.h"

class Breathing: public RGBEffect
{
public:
    Breathing() {};
    ~Breathing() {};

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QLayout*)                                override;
    void        StepEffect(std::vector<OwnedControllerAndZones> LocalControllers, int Step)  override;

    void        SetSpeed(int Speed)                                         override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override {};
    void        ASelectionWasChanged()                                      override {};
    void        ToggleRandomColors(bool RandomEnabled)                      override;

    void        SetWidth(int NewWidth);

    int                     GetSpeed()      override {return Speed;      };
    int                     GetSlider2Val() override {return 0;          };
    std::vector<RGBColor>   GetUserColors() override {return UserColors; };

    void                    EffectState(bool) override {return;};

    void                    LoadCustomSettings(json) override {return;};
    json                    SaveCustomSettings(json) override {return json();};

    EffectInfo EffectDetails;
private:
    float                   Speed;
    std::vector<RGBColor>   UserColors;
    float                   Progress = 255;
    bool                    Dir; /* true = down, false = up */
    bool RandomColors = false;
    bool RandomThisCycle;
    bool AlreadyMade = false;
    RGBColor RandomColor;
};

#endif // BREATHING_H
