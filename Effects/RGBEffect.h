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

struct OwnedControllerAndZones
{
    RGBController*      Controller;
    std::vector<int>    OwnedZones;
};

struct ZoneOwnedBy
{
    int Zone;
    std::string EffectName;
};

struct BetterController
{
    RGBController*              Controller;
    bool                        HasDirect;
    int                         DirectIndex;
    int                         Index;
    std::vector<ZoneOwnedBy>    OwnedZones;
};

class RGBEffect
{
public:
    virtual EffectInfo  DefineEffectDetails()                               = 0;
    virtual void        StepEffect(std::vector<OwnedControllerAndZones>, int Step)   = 0;

    virtual void        SetSpeed(int Speed)                                 = 0;
    virtual void        SetUserColors(std::vector<RGBColor>)                = 0;

    EffectInfo          EffectDetails;
};

#endif // RGBEFFECT_H
