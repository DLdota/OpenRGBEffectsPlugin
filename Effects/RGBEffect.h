#ifndef RGBEFFECT_H
#define RGBEFFECT_H

#include <QLayout>
#include "RGBController.h"
#include "json.hpp"

using json = nlohmann::json;

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

    bool HasCustomWidgets;
    bool HasCustomSettings;
};

struct OwnedControllerAndZones
{
    RGBController*     Controller;
    std::vector<unsigned int>   OwnedZones;
};

struct ZoneOwnedBy
{
    unsigned int Zone;
    unsigned int EffectIndex;
    std::string EffectName;
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
    virtual void        DefineExtraOptions(QLayout* Scaler)                          = 0;
    virtual void        StepEffect(std::vector<OwnedControllerAndZones>, int FPS)    = 0;

    virtual void        SetSpeed(int Speed)                                          = 0;
    virtual void        SetUserColors(std::vector<RGBColor>)                         = 0;
    virtual void        Slider2Changed(int)                                          = 0;
    virtual void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>)   = 0;
    virtual void        ToggleRandomColors(bool RandomEnabled)                       = 0;
    virtual int                     GetSpeed()                                       = 0;
    virtual int                     GetSlider2Val()                                  = 0;
    virtual std::vector<RGBColor>   GetUserColors()                                  = 0;

    virtual void                    EffectState(bool)                                = 0;

    virtual void                    LoadCustomSettings(json)                         = 0;
    virtual json                    SaveCustomSettings(json)                         = 0;

    EffectInfo          EffectDetails;
};

#endif // RGBEFFECT_H
