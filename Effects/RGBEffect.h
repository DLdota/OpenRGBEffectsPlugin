#ifndef RGBEFFECT_H
#define RGBEFFECT_H
#include "ORGBEffectPlugin.h"
#include "hsv.h"

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

struct BetterController
{
    RGBController*  Controller;
    int             Index;
    bool            Locked;
    std::string     OwnedBy;
};

class RGBEffect
{
public:
    virtual EffectInfo DefineEffectDetails()                    = 0;
    virtual void       StepEffect(std::vector<RGBController*>)  = 0;

    EffectInfo                  EffectDetails;
};

#endif // RGBEFFECT_H
