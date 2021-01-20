#include "RainbowWave.h"

EffectInfo RainbowWave::DefineEffectDetails()
{
    RainbowWave::EffectDetails.EffectName = "Rainbow Wave";
    RainbowWave::EffectDetails.EffectDescription = "A sliding Rainbow";

    RainbowWave::EffectDetails.IsReversable = true;
    RainbowWave::EffectDetails.MaxSpeed     = 30;
    RainbowWave::EffectDetails.MinSpeed     = 1;
    RainbowWave::EffectDetails.UserColors   = 0;

    RainbowWave::EffectDetails.MinSlider2Val = 5;
    RainbowWave::EffectDetails.MaxSlider2Val = 50;
    RainbowWave::EffectDetails.Slider2Name   = "Width";

    return RainbowWave::EffectDetails;
}

void RainbowWave::DefineExtraOptions(QWidget* Parent)
{
    return;
}

void RainbowWave::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    hsv_t HSVVal;
    HSVVal.saturation = 255;
    HSVVal.value      = 255;

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;
            if (ZT == ZONE_TYPE_SINGLE)
            {

            }
            else if (ZT == ZONE_TYPE_LINEAR)
            {
                int SetLEDIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;
                for (int LedID = 0; LedID < int(Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count); LedID++)
                {
                    int HUE = ((Progress + LedID) * Width);

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
        Progress += float(float(Speed) / float(FPS));
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

void RainbowWave::Slider2Changed(int NewWidth)
{
    Width = NewWidth;
}
/*--------------*\
| Extra Options  |
\*--------------*/