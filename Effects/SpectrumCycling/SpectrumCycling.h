#ifndef SPECTRUMCYCLING_H
#define SPECTRUMCYCLING_H

#include "RGBEffect.h"
#include "OpenRGBEffectPage.h"

class SpectrumCycling:
public RGBEffect
{
public:
    SpectrumCycling() {};
    ~SpectrumCycling() {};
    EffectInfo DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QLayout*)                               override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int Step) override;

    void        SetSpeed(int Speed)                                        override;
    void        SetUserColors(std::vector<RGBColor>)                       override;
    void        Slider2Changed(int)                                        override;
    void        ASelectionWasChanged()                                     override;
    void        ToggleRandomColors(bool)                                   override {};

    int                     GetSpeed()      override {return Speed;  };
    int                     GetSlider2Val() override {return 0;      };
    std::vector<RGBColor>   GetUserColors() override {return {};     };

    void                    EffectState(bool)override {return;};

    void                    LoadCustomSettings(json) override {return;};
    json                    SaveCustomSettings(json) override {return json();};

    EffectInfo EffectDetails;
private:
    float  CurrentHue = 0.0000;
    int    Speed;
};

#endif // SPECTRUMCYCLING_H
