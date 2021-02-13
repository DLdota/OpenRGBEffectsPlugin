#ifndef AMBIENT_H
#define AMBIENT_H

#include "RGBEffect.h"
#include "ScreenSelection.h"

class Ambient : public RGBEffect
{
public:
    Ambient() {};
    ~Ambient() {};

    EffectInfo              DefineEffectDetails()                                       override;
    void                    DefineExtraOptions(QLayout* Parent)                         override;
    void                    StepEffect(std::vector<OwnedControllerAndZones>, int FPS)   override;

    void                    SetSpeed(int)                           override {};
    void                    Slider2Changed(int)                     override {};
    void                    SetUserColors(std::vector<RGBColor>)    override {};
    void                    ASelectionWasChanged()                  override {};
    void                    ToggleRandomColors(bool)                override {};

    int                     GetSpeed()                              override {return 0;}  ;
    int                     GetSlider2Val()                         override {return 0;}  ;
    std::vector<RGBColor>   GetUserColors()                         override {return {};} ;

    void                    LoadCustomSettings(json)                override;
    json                    SaveCustomSettings(json)                override;

    void                    EffectState(bool)                       override;

private:
    EffectInfo EffectDetails;
    ScreenSelection* SCRNSLCT;
};

#endif // AMBIENT_H
