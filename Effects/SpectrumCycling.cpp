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

void SpectrumCycling::StartEffect()
{
    qDebug() << "Effect started";
    hsv_t HSVVal;
    HSVVal.value = 255;
    HSVVal.saturation = 255;
    while (true)
    {
        for (int HUE = 0; HUE < 360; HUE += Speed / 100 )
        {
            HSVVal.hue = HUE;
            for (int i = 0; i < int(OwnedDevices.size()); i++)
            {
                if (!StopNextLoop)
                {

                    OwnedDevices[i]->SetAllLEDs(hsv2rgb(&HSVVal));
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                else
                {
                    return;
                }
            }
        }
    }
}

void SpectrumCycling::StopEffect()
{
    qDebug() << "Effect Stopped";
}

void SpectrumCycling::SetSpeed(int Speed)
{
    SpectrumCycling::Speed = Speed;
}

void SpectrumCycling::SetUserColors(std::vector<UserColor>)
{
    /*--------------------------------------*\
    | This mode doesn't support user colors  |
    \*--------------------------------------*/
}
