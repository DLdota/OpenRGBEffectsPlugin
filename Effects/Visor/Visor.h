#ifndef Visor_H
#define Visor_H

#include "hsv.h"
#include "RGBEffect.h"
#include "OpenRGBEffectPage.h"

class Visor: public RGBEffect
{
public:
    Visor() {};
    ~Visor() {};

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QLayout*)                                override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int)       override;

    void        SetSpeed(int Speed)                                         override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override;
    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>)  override {};
    void        ToggleRandomColors(bool)                                    override;

    void                    SetWidth(int NewWidth);
    int                     GetSpeed()                                      override {return Speed;      };
    int                     GetSlider2Val()                                 override {return 0;          };
    std::vector<RGBColor>   GetUserColors()                                 override {return UserColors; };

    void            EffectState(bool)                                       override {return;            };

    void            LoadCustomSettings(json)                                override {return;            };
    json            SaveCustomSettings(json)                                override {return json();     };

    EffectInfo EffectDetails;

private:
    float                   Speed = 10;
    std::vector<RGBColor>   UserColors;    
    bool                    Random = false;
    bool                    Dir = true;
    int                     width = 10;
    float                   Progress = 0;
    hsv_t                   Head;
    hsv_t                   Tail;
    float                   current_head_hue;
    float                   current_tail_hue;

    RGBColor GetColor(int i, int count);
    void     GenerateRandomColors();

};

#endif // Visor_H

