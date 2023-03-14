#include "SpectrumCycling.h"
#include "hsv.h"

REGISTER_EFFECT(SpectrumCycling);

SpectrumCycling::SpectrumCycling() : RGBEffect()
{
    EffectDetails.EffectName = "Spectrum Cycling";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Goes through every solid color of the rainbow";
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.SupportsRandom = false;

    SetSpeed(50);
}

void SpectrumCycling::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    hsv_t HSVVal;

    HSVVal.value = 255;
    HSVVal.saturation = 255;
    HSVVal.hue = int(progress) % 360;

    RGBColor color = RGBColor(hsv2rgb(&HSVVal));

    for(ControllerZone* controller_zone: controller_zones)
    {
        controller_zone->SetAllZoneLEDs(color, Brightness);
    }

    progress += (float) Speed / (float) FPS;
}

