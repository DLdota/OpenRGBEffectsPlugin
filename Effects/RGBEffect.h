#ifndef RGBEFFECT_H
#define RGBEFFECT_H

#include <QLayout>
#include "ControllerZone.h"
#include "json.hpp"

using json = nlohmann::json;

struct EffectInfo
{
    std::string EffectName;
    std::string EffectClassName;
    std::string EffectDescription;
    int         EffectIndex;

    bool IsReversable;
    int  MaxSpeed;
    int  MinSpeed;

    bool AllowOnlyFirst;
    unsigned int  UserColors;

    int         MaxSlider2Val;
    int         MinSlider2Val;
    std::string Slider2Name;

    bool HasCustomWidgets;
    bool HasCustomSettings;
};

class RGBEffect
{
public:
    virtual ~RGBEffect(){};

    virtual void DefineExtraOptions(QLayout*) {}
    virtual void StepEffect(std::vector<ControllerZone>) {};
    virtual void ASelectionWasChanged(std::vector<ControllerZone>) {}

    virtual void EffectState(bool Enabled) { EffectEnabled = Enabled; }

    virtual void LoadCustomSettings(json) {}
    virtual json SaveCustomSettings(json) { return json(); }

    virtual void SetAutoStart(bool value) { AutoStart = value;}
    virtual bool IsAutoStart() { return AutoStart;}

    virtual void SetFPS(unsigned int value) { FPS = value; }
    virtual unsigned int GetFPS()  { return FPS; }

    virtual void SetSpeed(unsigned int value) { Speed = value; }
    virtual unsigned int GetSpeed() { return Speed; };

    virtual void SetUserColors(std::vector<RGBColor> colors) { UserColors = colors; }
    virtual std::vector<RGBColor> GetUserColors() {return UserColors; }

    virtual void SetRandomColorsEnabled(bool value) { RandomColorsEnabled = value; }
    virtual bool IsRandomColorsEnabled() { return RandomColorsEnabled; }

    virtual void SetOnlyFirstColorEnabled(bool value) {OnlyFirstColorEnabled = value; }
    virtual bool IsOnlyFirstColorEnabled() { return OnlyFirstColorEnabled; }

    virtual unsigned int GetSlider2Val() { return Slider2Val; }
    virtual void SetSlider2Val(unsigned int value) { Slider2Val = value; }

    EffectInfo EffectDetails;

protected:
    unsigned int FPS = 60;
    unsigned int Speed = 1;
    unsigned int Slider2Val = 1;
    std::vector<RGBColor> UserColors;
    bool RandomColorsEnabled = false;
    bool AutoStart = false;
    bool OnlyFirstColorEnabled = false;
    bool EffectEnabled = false;
};

#endif // RGBEFFECT_H
