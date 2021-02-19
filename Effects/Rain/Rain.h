#ifndef RAIN_H
#define RAIN_H

#include "RGBEffect.h"

struct Drops
{
    int ControllerIndex;
    int  ZoneIndex;

    zone_type   ZT;
    int   LEDCount;
    int   StartingLED;

    bool  Reversed;

    int    Collumn;
    int    CollumnCount;
    float  Progress;

    RGBColor C;
};

class Rain: public RGBEffect
{
public:
    Rain() {};
    ~Rain() {};

    EffectInfo  DefineEffectDetails()                                      override;
    void        DefineExtraOptions(QLayout*)                               override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int)      override;

    void        SetSpeed(int Speed)                                        override;
    void        SetUserColors(std::vector<RGBColor>)                       override;
    void        Slider2Changed(int)                                        override;
    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>) override;
    void        ToggleRandomColors(bool RandomEnabled)                     override;

    int                     GetSpeed()                                     override;
    int                     GetSlider2Val()                                override;
    std::vector<RGBColor>   GetUserColors()                                override;

    void                    EffectState(bool)                              override {return;};

    void                    LoadCustomSettings(json)                       override {return;        };
    json                    SaveCustomSettings(json)                       override {return json(); };

    EffectInfo              EffectDetails;

private:
    float Speed; // Float so that it defaults to float opperations when doing devision by FPS
    int DropCount;

    std::vector<Drops> CurrentDrops;

    RGBColor UserColor;
    bool     RandomColors = false;

    std::vector<std::vector<bool>> HasEffect;
};

#endif // RAIN_H
