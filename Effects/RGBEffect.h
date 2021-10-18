#ifndef RGBEFFECT_H
#define RGBEFFECT_H

#include <QWidget>
#include <QLayout>
#include "ControllerZone.h"
#include "json.hpp"

using json = nlohmann::json;

struct EffectInfo
{
    std::string EffectName = "";
    std::string EffectClassName = "";
    std::string EffectDescription = "";

    bool IsReversable = false;
    int  MaxSpeed = 0;
    int  MinSpeed = 0;

    bool AllowOnlyFirst = false;
    unsigned int  UserColors = 0;

    int         MaxSlider2Val = 0;
    int         MinSlider2Val = 0;
    std::string Slider2Name = "";

    bool HasCustomSettings = false;
};

class RGBEffect : public QWidget
{
    Q_OBJECT

public:
    explicit RGBEffect(QWidget* parent = nullptr) : QWidget(parent){};
    virtual ~RGBEffect(){};

    // todo: rename this method
    // => LoadCustomLayout
    virtual void DefineExtraOptions(QLayout*) {}
    virtual void StepEffect(std::vector<ControllerZone*>) {};
    virtual void OnControllerZonesListChanged(std::vector<ControllerZone*>) {}

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

    virtual unsigned int GetBrightness() { return Brightness; }
    virtual void SetBrightness(unsigned int value) { Brightness = value; }

    EffectInfo EffectDetails;

    void EmitMeasure(float t, int d){ emit TimeMeasured(t, d); }

signals:
    void TimeMeasured(float, int);

protected:
    unsigned int FPS = 60;
    unsigned int Speed = 1;
    unsigned int Slider2Val = 1;
    std::vector<RGBColor> UserColors;
    bool RandomColorsEnabled = false;
    bool AutoStart = false;
    bool OnlyFirstColorEnabled = false;
    bool EffectEnabled = false;
    float Brightness = 100;
};

#endif // RGBEFFECT_H
