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

struct UserColor
{
    int R;
    int G;
    int B;
};

class RGBEffect
{
public:
    virtual EffectInfo  DefineEffectDetails()                   = 0;
    virtual void        StartEffect()                           = 0;
    virtual void        StopEffect()                            = 0;

    virtual void        SetSpeed(int Speed)                     = 0;
    virtual void        SetUserColors(std::vector<UserColor>)   = 0;

    std::vector<RGBController*> OwnedDevices;
    EffectInfo                  EffectDetails;
};

#endif // RGBEFFECT_H
