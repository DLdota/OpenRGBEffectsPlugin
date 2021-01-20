#ifndef RGBEFFECT_H
#define RGBEFFECT_H

#pragma once

#include "ORGBEffectPlugin.h"
#include "hsv.h"

struct EffectInfo
{
    std::string EffectName;
    std::string EffectDescription;
    int         EffectIndex;

    bool IsReversable;
    int  MaxSpeed;
    int  MinSpeed;
    int  UserColors;

    int         MaxSlider2Val;
    int         MinSlider2Val;
    std::string Slider2Name;
};

struct OwnedControllerAndZones
{
    RGBController*     Controller;
    std::vector<int>   OwnedZones;
};

struct ZoneOwnedBy
{
    int Zone;
    std::string EffectName;
    bool IsReversed;
};

struct BetterController
{
    RGBController*              Controller;
    bool                        HasDirect;
    int                         DirectIndex;
    int                         Index;
    std::vector<ZoneOwnedBy>    OwnedZones;
    std::vector<bool>           ReversedZones;
};

class RGBEffect
{
public:
    virtual EffectInfo  DefineEffectDetails()                                        = 0;
    virtual void        DefineExtraOptions(QWidget* Parent)                          = 0;
    virtual void        StepEffect(std::vector<OwnedControllerAndZones>, int Step)   = 0;

    virtual void        SetSpeed(int Speed)                                          = 0;
    virtual void        SetUserColors(std::vector<RGBColor>)                         = 0;
    virtual void        Slider2Changed(int)                                          = 0;

    EffectInfo          EffectDetails;
};

#endif // RGBEFFECT_H
