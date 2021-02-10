#ifndef SEESAWMOTION_H
#define SEESAWMOTION_H

#include "RGBEffect.h"
#include "OpenRGBEffectPage.h"

class SeesawMotion: public RGBEffect
{
public:
    SeesawMotion() {};
    ~SeesawMotion() {};

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QLayout*)                                override;
    void        StepEffect(std::vector<OwnedControllerAndZones> LocalControllers, int Step)  override;

    void        SetSpeed(int Speed)                                         override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override;
    void        ASelectionWasChanged()                                      override;

    void        SetWidth(int NewWidth);

    int                     GetSpeed()      override  {return Speed;      };
    int                     GetSlider2Val() override  {return 0;          };
    std::vector<RGBColor>   GetUserColors() override  {return UserColors; };

    void            EffectState(bool)       override  {return;};

    void            LoadCustomSettings(json) override {return;};
    json            SaveCustomSettings(json) override {return json();};


    EffectInfo EffectDetails;
private:
    float                   Speed;
    std::vector<RGBColor>   UserColors;
    float                   Progress = 100;
    bool                    Dir; /* true = down, false = up */
    int                     width = 2;
};

#endif // SeesawMotion_H
