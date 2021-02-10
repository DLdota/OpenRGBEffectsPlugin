#include "RainbowWave.h"
#include "OpenRGBEffectTab.h"

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

    RainbowWave::EffectDetails.HasCustomWidgets = false;
    RainbowWave::EffectDetails.HasCustomSettings = false;

    return RainbowWave::EffectDetails;
}

void RainbowWave::DefineExtraOptions(QLayout*){}

void RainbowWave::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    hsv_t HSVVal;
    HSVVal.saturation = 255;
    HSVVal.value      = 255;

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            /*-------------------*\
            | Setup for the loop  |
            \*-------------------*/
            int SetLEDIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;
            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;
            bool RVRS = OpenRGBEffectTab::CheckReversed(ControllerID, Controllers[ControllerID].OwnedZones[ZoneID]);

            /*----------------------------------------------------*\
            | Adjust how it applies for the specific type of zone  |
            \*----------------------------------------------------*/
            if (ZT == ZONE_TYPE_SINGLE)
            {
                int HUE = (Progress * Width);
                HSVVal.hue = HUE;
                for (int LedID = 0; LedID < int(Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count); LedID++)
                {
                    Controllers[ControllerID].Controller->SetLED((SetLEDIndex + LedID),RGBColor(hsv2rgb(&HSVVal)));
                }
            }

            else if (ZT == ZONE_TYPE_LINEAR)
            {
                int LEDCOUNT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;
                for (int LedID = 0; LedID < LEDCOUNT; LedID++)
                {
                    int HUE;
                    if (RVRS) HUE = ((Progress + ( (LEDCOUNT - 1) - LedID) ) * Width);
                    else HUE = ((Progress + LedID) * Width);

                    HSVVal.hue = HUE;

                    Controllers[ControllerID].Controller->SetLED((SetLEDIndex+LedID),RGBColor(hsv2rgb(&HSVVal)));
                }
            }

            else if (ZT == ZONE_TYPE_MATRIX)
            {
                int CollumnCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                int RowCount     = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                for (int CollumnID = 0; CollumnID < CollumnCount; CollumnID++)
                {
                    int HUE;
                    if (RVRS)
                    {
                        HUE = ((Progress + (int)( (CollumnCount - 1) - CollumnID)) * Width);
                    }
                    else
                    {
                        HUE = ((Progress + (int)CollumnID) * Width);
                    }

                    HSVVal.hue = HUE;

                    for (int RowID = 0; RowID < RowCount; RowID++)
                    {
                        int LedID = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->map[((RowID * CollumnCount) + CollumnID)];
                        Controllers[ControllerID].Controller->SetLED(SetLEDIndex + LedID,RGBColor(hsv2rgb(&HSVVal)));
                    }
                }
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

void RainbowWave::ASelectionWasChanged()
{

}
