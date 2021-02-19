#ifndef BREATHING_H
#define BREATHING_H

#include "RGBEffect.h"

class Breathing: public RGBEffect
{
public:
    Breathing() {};
    ~Breathing() {};

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QLayout*)                                override {};
    void        StepEffect(std::vector<OwnedControllerAndZones>, int)       override;

    void        SetSpeed(int Speed)                                         override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override {};
    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>)  override {};
    void        ToggleRandomColors(bool RandomEnabled)                      override;

    void        SetWidth(int NewWidth);

    int                     GetSpeed()                                      override {return Speed;      };
    int                     GetSlider2Val()                                 override {return 0;          };
    std::vector<RGBColor>   GetUserColors()                                 override {return UserColors; };

    void                    EffectState(bool)                               override {return;            };

    void                    LoadCustomSettings(json)                        override {return;            };
    json                    SaveCustomSettings(json)                        override {return json();     };

    EffectInfo EffectDetails;
private:
    float                   Speed;
    std::vector<RGBColor>   UserColors;
    float                   Progress = 0;
    bool                    GoingUp = false; /* true = down, false = up */
    bool                    RandomColors = false;
    bool                    AlreadyMade = false;
    bool                    RandomThisCycle;
    RGBColor                RandomColor;
};

#endif // BREATHING_H
