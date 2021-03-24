#ifndef LIGHTNING_H
#define LIGHTNING_H

#include <stdlib.h>
#include "RGBEffect.h"


class Lightning: public QWidget, public RGBEffect
{
    Q_OBJECT

public:
    explicit Lightning(QWidget *parent = nullptr);
    ~Lightning();

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QLayout*)                                override {};
    void        StepEffect(std::vector<OwnedControllerAndZones>, int)       override;
    void        SetSpeed(int)                                               override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override;
    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>)  override;
    void        ToggleRandomColors(bool)                                    override;

    int                     GetSpeed()                                      override {return Speed;      };
    int                     GetSlider2Val()                                 override {return Decay;      };
    std::vector<RGBColor>   GetUserColors()                                 override {return UserColors; };

    void                    EffectState(bool)                               override {return;            };

    void                    LoadCustomSettings(json)                        override {};
    json                    SaveCustomSettings(json)                        override {return json{};};

    EffectInfo EffectDetails;

private:
    std::vector<RGBColor>   UserColors;
    hsv_t UserHSV;

    std::vector<hsv_t> Lightnings;
    bool Random = false;
    unsigned int Speed = 1;
    unsigned int Decay = 10;

    RGBColor TriggerLightning(int, int);

};

#endif // LIGHTNING_H
