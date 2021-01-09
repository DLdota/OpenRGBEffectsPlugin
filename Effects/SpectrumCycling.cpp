#include "SpectrumCycling.h"

EffectInfo SpectrumCycling::DefineEffectDetails()
{
    SpectrumCycling::EffectDetails.EffectName = "Spectrum Cycling";
    SpectrumCycling::EffectDetails.EffectDescription = "Goes through every solid color";

    SpectrumCycling::EffectDetails.IsReversable = false;
    SpectrumCycling::EffectDetails.MaxSpeed     = 100;
    SpectrumCycling::EffectDetails.MinSpeed     = 0;
    SpectrumCycling::EffectDetails.UserColors   = 0;

    return SpectrumCycling::EffectDetails;
}

void SpectrumCycling::StartEffect()
{
    qDebug() << "Effect started";
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
