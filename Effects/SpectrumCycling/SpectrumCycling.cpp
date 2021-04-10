#include <iostream>
#include "SpectrumCycling.h"
#include "hsv.h"

EffectInfo SpectrumCycling::DefineEffectDetails()
{
    SpectrumCycling::EffectDetails.EffectName = "Spectrum Cycling";
    SpectrumCycling::EffectDetails.EffectDescription = "Goes through every solid color";

    SpectrumCycling::EffectDetails.IsReversable = false;
    SpectrumCycling::EffectDetails.MaxSpeed     = 100;
    SpectrumCycling::EffectDetails.MinSpeed     = 1;
    SpectrumCycling::EffectDetails.UserColors   = 0;

    SpectrumCycling::EffectDetails.MaxSlider2Val = 0;
    SpectrumCycling::EffectDetails.MinSlider2Val = 0;
    SpectrumCycling::EffectDetails.Slider2Name   = "";

    SpectrumCycling::EffectDetails.HasCustomWidgets = false;
    SpectrumCycling::EffectDetails.HasCustomSettings = false;

    return SpectrumCycling::EffectDetails;
}

void SpectrumCycling::StepEffect(std::vector<OwnedControllerAndZones> PassedTo, int FPS)
{
    hsv_t HSVVal;
    HSVVal.value = 255;
    HSVVal.saturation = 255;

    HSVVal.hue = CurrentHue;
    for (int i = 0; i < int(PassedTo.size()); i++)
    {
        for (int ZoneID = 0; ZoneID < int(PassedTo[i].OwnedZones.size()); ZoneID++)
        {
            PassedTo[i].Controller->SetAllZoneLEDs(PassedTo[i].OwnedZones[ZoneID],RGBColor(hsv2rgb(&HSVVal)));
        }
    }


    if (CurrentHue < 360)
    {
        CurrentHue = CurrentHue + ((float)Speed / (float)FPS);
    }
    else
    {
        CurrentHue = 0.0000;
    }

}

void SpectrumCycling::SetSpeed(int Speed)
{
    SpectrumCycling::Speed = Speed;
}

