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
    qDebug() << "RBWave called";
    for (int DeviceIndex = 0; DeviceIndex < int(Controllers.size()); DeviceIndex++)
    {
        for (int ZoneIndex = 0; ZoneIndex < int(Controllers[DeviceIndex]->zones.size()); ZoneIndex++)
        {
            zone_type ZoneType = Controllers[DeviceIndex]->zones[ZoneIndex].type;
            for (int LEDIndex = 0; LEDIndex < int(Controllers[DeviceIndex]->zones[ZoneIndex].leds_count); LEDIndex++)
            {
                if ( ZoneType == ZONE_TYPE_SINGLE)
                {
                    //HUE = 1000 %
                }
                else if (ZoneType == ZONE_TYPE_LINEAR)
                {

                }
                else if ( ZoneType == ZONE_TYPE_MATRIX)
                {

                }
            }
        }
    }
    if (Progress < 100)
    {
        Progress += Speed;
    }
    else if (Progress >= 100)
    {
        Progress = 0;
    }
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
