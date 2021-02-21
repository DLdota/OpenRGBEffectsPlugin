#ifndef RAVE_H
#define RAVE_H

#include "OpenRGBEffectTab.h"
#include "RGBEffect.h"

struct LightList
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
    int ColorIndex;
};

class Rave: public RGBEffect
{
public:
    Rave() {};
    EffectInfo  DefineEffectDetails()                  override;
    void        DefineExtraOptions(QLayout*)           override {};
    void        StepEffect(std::vector<OwnedControllerAndZones>, int FPS) override;

    void        SetSpeed(int Speed)                    override;
    void        SetUserColors(std::vector<RGBColor>)   override;
    void        Slider2Changed(int)                    override;
    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>) override;
    void        ToggleRandomColors(bool RandomEnabled) override;
    int                     GetSpeed()                 override {return Speed;};
    int                     GetSlider2Val()            override {return LightCount;};
    std::vector<RGBColor>   GetUserColors()            override {return UserColors;};

    void                    EffectState(bool)          override{};

    void                    LoadCustomSettings(json)   override {};
    json                    SaveCustomSettings(json)   override {return nullptr;};

    EffectInfo              EffectDetails;

private:
    float Speed; // Float so that it defaults to float opperations when doing devision by FPS
    int LightCount;

    std::vector<LightList> CurrentLights;

    std::vector<RGBColor> UserColors;
    bool RandomColors = false;

    std::vector<std::vector<std::vector<bool>>> HasEffect;
};

#endif // RAVE_H
