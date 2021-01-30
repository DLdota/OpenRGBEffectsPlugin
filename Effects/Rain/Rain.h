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
    float Progress;
};

class Rain: public RGBEffect
{
public:
    Rain() {};
    ~Rain() {};

    EffectInfo  DefineEffectDetails()                                     override;
    void        DefineExtraOptions(QWidget* Parent)                       override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int FPS) override;

    void        SetSpeed(int Speed)                                       override;
    void        SetUserColors(std::vector<RGBColor>)                      override;
    void        Slider2Changed(int)                                       override;
    void        ASelectionWasChanged()                                    override;

    int                     GetSpeed()                                    override;
    int                     GetSlider2Val()                               override;
    std::vector<RGBColor>   GetUserColors()                               override;

    EffectInfo              EffectDetails;


private:
    float Speed; // Float so that it defaults to float opperations when doing devision by FPS
    int DropCount;

    std::vector<Drops> CurrentDrops;

    RGBColor UserColor;

};

#endif // RAIN_H
