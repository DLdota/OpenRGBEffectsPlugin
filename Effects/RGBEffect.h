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

    bool AllowOnlyFirst = false;
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
    virtual void        DefineExtraOptions(QLayout*) {};
    virtual void        StepEffect(std::vector<OwnedControllerAndZones>, int FPS)    = 0;

    virtual void        SetSpeed(int SPD)                       {Speed = SPD;};
    virtual void        SetUserColors(std::vector<RGBColor> UC) {UserColors = UC;                     };
    virtual void        Slider2Changed(int SVal)                {Slider2Val = SVal;                   };
    virtual void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>)   = 0;
    virtual void        ToggleRandomColors(bool RandomEnabled)  {RandomColorsEnabled = RandomEnabled; };
    virtual void        OnlyFirstChange(bool OnlyFirst)         {OnlyFirstColorEnabled = OnlyFirst;   };
    virtual int                     GetSpeed()                  {return Speed;                        };
    virtual int                     GetSlider2Val()             {return Slider2Val;                   };
    virtual std::vector<RGBColor>   GetUserColors()             {return UserColors;                   };

    virtual void                    EffectState(bool Enabled)   {EffectEnabled = Enabled;             };

    virtual void                    LoadCustomSettings(json)                         = 0;
    virtual json                    SaveCustomSettings(json)                         = 0;

    EffectInfo          EffectDetails;
protected:
    float Speed = 1;
    float Slider2Val = 1;

    std::vector<RGBColor> UserColors;
    bool RandomColorsEnabled = false;
    bool OnlyFirstColorEnabled = false;

    bool EffectEnabled;
};

#endif // RGBEFFECT_H
