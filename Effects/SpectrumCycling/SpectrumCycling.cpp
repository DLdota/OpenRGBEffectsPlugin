#include "SpectrumCycling.h"
#include "hsv.h"

REGISTER_EFFECT(SpectrumCycling);

SpectrumCycling::SpectrumCycling() : RGBEffect()
{
    EffectDetails.EffectName = "Spectrum Cycling";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Goes through every solid color";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 0;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = false;
    EffectDetails.HasCustomSettings = false;
}

void SpectrumCycling::StepEffect(std::vector<ControllerZone> controller_zones)
{
    hsv_t HSVVal;

    HSVVal.value = 255;
    HSVVal.saturation = 255;
    HSVVal.hue = CurrentHue;

    for(ControllerZone& controller_zone: controller_zones)
    {
        controller_zone.controller->SetAllZoneLEDs(controller_zone.zone_idx, RGBColor(hsv2rgb(&HSVVal)));
    }

    CurrentHue += ((float)Speed / (float)FPS);

    if(CurrentHue >= 360)
    {
        CurrentHue = 0;
    }
}

