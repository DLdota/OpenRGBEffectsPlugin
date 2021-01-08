#include "SpectrumCycling.h"

EffectInfo SpectrumCycling::DefineEffectDetails()
{
    SpectrumCycling::PluginDetails.EffectName = "Spectrum Cycling";
    SpectrumCycling::PluginDetails.EffectDescription = "Goes through every solid color";

    SpectrumCycling::PluginDetails.IsReversable = false;
    SpectrumCycling::PluginDetails.MaxSpeed     = 100;
    SpectrumCycling::PluginDetails.MinSpeed     = 0;
    SpectrumCycling::PluginDetails.UserColors   = 0;

    return SpectrumCycling::PluginDetails;
}

void SpectrumCycling::StartEffect()
{

}

void SpectrumCycling::StopEffect()
{

}
