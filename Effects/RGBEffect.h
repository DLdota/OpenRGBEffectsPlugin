#ifndef RGBEFFECT_H
#define RGBEFFECT_H
#include "ORGBEffectPlugin.h"

struct EffectInfo
{
    std::string EffectName;
    std::string EffectDescription;

    bool IsReversable;
    int  MaxSpeed;
    int  MinSpeed;
    int  UserColors;
};

class RGBEffect
{
public:
    virtual EffectInfo DefineEffectDetails() = 0;
    virtual void       StartEffect()         = 0;
    virtual void       StopEffect()          = 0;

    std::vector<RGBController*> OwnedDevices;
    EffectInfo                  EffectDetails;
};

#endif // RGBEFFECT_H
