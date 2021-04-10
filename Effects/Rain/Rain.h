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
    void        StepEffect(std::vector<OwnedControllerAndZones>, int)      override;

    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>) override;
    void                    EffectState(bool)                              override {};

    void                    LoadCustomSettings(json)                       override {};
    json                    SaveCustomSettings(json)                       override {return json(); };

    EffectInfo              EffectDetails;
private:
    //int DropCount;
    std::vector<Drops> CurrentDrops;
    int      PrevNum;
    std::vector<std::vector<bool>> HasEffect;
};

#endif // RAIN_H
