#include "SpectrumCycling.h"

EffectInfo SpectrumCycling::DefineEffectDetails()
{
    SpectrumCycling::EffectDetails.EffectName = "Spectrum Cycling";
    SpectrumCycling::EffectDetails.EffectDescription = "Goes through every solid color";

    SpectrumCycling::EffectDetails.IsReversable = false;
    SpectrumCycling::EffectDetails.MaxSpeed     = 10;
    SpectrumCycling::EffectDetails.MinSpeed     = 1;
    SpectrumCycling::EffectDetails.UserColors   = 0;

    return SpectrumCycling::EffectDetails;
}

void SpectrumCycling::StepEffect(std::vector<RGBController*> PassedTo, int Step)
{
    if (Step%Speed == 0)
    {
        hsv_t HSVVal;
        HSVVal.value = 255;
        HSVVal.saturation = 255;

        HSVVal.hue = CurrentHue;
        qDebug() << HSVVal.hue;
        for (int i = 0; i < int(PassedTo.size()); i++)
        {
            PassedTo[i]->SetAllLEDs(RGBColor(hsv2rgb(&HSVVal)));
        }
        if (CurrentHue < 1000)
        {
            CurrentHue += Speed;
        }
        else
        {
            CurrentHue = 0;
        }
    }
    return;
}

void SpectrumCycling::SetSpeed(int Speed)
{
    SpectrumCycling::Speed = Speed;
}

void SpectrumCycling::SetUserColors(std::vector<RGBColor>)
{
    /*------------------------------------------------*\
    | This effect does not support user chosen colors  |
    \*------------------------------------------------*/
}
