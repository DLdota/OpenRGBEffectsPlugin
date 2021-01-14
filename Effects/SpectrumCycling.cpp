#include "SpectrumCycling.h"

EffectInfo SpectrumCycling::DefineEffectDetails()
{
    SpectrumCycling::EffectDetails.EffectName = "Spectrum Cycling";
    SpectrumCycling::EffectDetails.EffectDescription = "Goes through every solid color";

    SpectrumCycling::EffectDetails.IsReversable = false;
    SpectrumCycling::EffectDetails.MaxSpeed     = 100;
    SpectrumCycling::EffectDetails.MinSpeed     = 1;
    SpectrumCycling::EffectDetails.UserColors   = 0;

    return SpectrumCycling::EffectDetails;
}

void SpectrumCycling::StepEffect(std::vector<RGBController*> PassedTo)
{
    hsv_t HSVVal;
    HSVVal.value = 255;
    HSVVal.saturation = 255;

    HSVVal.hue = CurrentProgress;
    qDebug() << HSVVal.hue;
    qDebug() << HSVVal.saturation;
    qDebug() << HSVVal.value;
    for (int i = 0; i < int(PassedTo.size()); i++)
    {
        PassedTo[i]->SetAllLEDs(RGBColor(hsv2rgb(&HSVVal)));
    }
    //CurrentProgress +=
    return;
}
