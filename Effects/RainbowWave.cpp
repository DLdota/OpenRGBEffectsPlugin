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

void RainbowWave::StepEffect(std::vector<RGBController*> Controllers)
{
    // Nothing in here yet.
}
