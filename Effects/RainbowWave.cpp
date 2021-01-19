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

void RainbowWave::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    hsv_t HSVVal;
    HSVVal.saturation = 255;
    HSVVal.value      = 255;

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        qDebug() << Controllers[ControllerID].OwnedZones.size();
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;
            qDebug() << ZT;
            if (ZT == ZONE_TYPE_SINGLE)
            {

            }
            else if (ZT == ZONE_TYPE_LINEAR)
            {
                qDebug() << "linear zone";
                int SetLEDIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;
                for (int LedID = 0; LedID < int(Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count); LedID++)
                {
                    uint HUE = ((Progress + LedID) * Speed);

                    HSVVal.hue = HUE;

                    Controllers[ControllerID].Controller->SetLED((SetLEDIndex+LedID),RGBColor(hsv2rgb(&HSVVal)));
                }
            }
            else if (ZT == ZONE_TYPE_MATRIX)
            {

            }
        }
    }

    if (Progress < 360)
    {
        Progress += float(float(1) * float(FPS));
    }
    else if (Progress >= 360)
    {
        Progress = 0;
    }
    return;
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
