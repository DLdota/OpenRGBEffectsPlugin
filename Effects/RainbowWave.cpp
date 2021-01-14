#include "RainbowWave.h"

EffectInfo RainbowWave::DefineEffectDetails()
{
    RainbowWave::EffectDetails.EffectName = "Rainbow Wave";
    RainbowWave::EffectDetails.EffectDescription = "A sliding Rainbow";

    RainbowWave::EffectDetails.IsReversable = true;
    RainbowWave::EffectDetails.MaxSpeed     = 30;
    RainbowWave::EffectDetails.MinSpeed     = 1;
    RainbowWave::EffectDetails.UserColors   = 0;

    return RainbowWave::EffectDetails;
}

void RainbowWave::StepEffect(std::vector<RGBController*> Controllers, int Step)
{
    // Nothing in here yet.
}

void RainbowWave::SetSpeed(int Speed)
{
    RainbowWave::Speed = Speed;
}

void RainbowWave::SetUserColors(std::vector<RGBColor>)
{
    /*------------------------------------------------*\
    | This effect does not support user chosen colors  |
    \*------------------------------------------------*/
}
