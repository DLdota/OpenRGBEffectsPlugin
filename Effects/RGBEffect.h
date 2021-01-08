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
    virtual ~RGBEffect() {};
    virtual EffectInfo DefineEffectDetails();
    virtual void       StartEffect();
    virtual void       StopEffect();

    std::vector<RGBController*> OwnedDevices;
    EffectInfo                  PluginDetails;
};

#endif // RGBEFFECT_H
