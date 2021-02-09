#include "SeesawMotion.h"
#include "OpenRGBEffectTab.h"

RGBColor OFF = ToRGBColor(0,0,0);

EffectInfo SeesawMotion::DefineEffectDetails()
{
    SeesawMotion::EffectDetails.EffectName = "Seesaw Motion";
    SeesawMotion::EffectDetails.EffectDescription = "A back and forth effect motion";

    SeesawMotion::EffectDetails.IsReversable = true;
    SeesawMotion::EffectDetails.MaxSpeed     = 200;
    SeesawMotion::EffectDetails.MinSpeed     = 1;
    SeesawMotion::EffectDetails.UserColors   = 2;

    SeesawMotion::EffectDetails.MinSlider2Val = 2;
    SeesawMotion::EffectDetails.MaxSlider2Val = 20;
    SeesawMotion::EffectDetails.Slider2Name   = "Width";

    return SeesawMotion::EffectDetails;
}

void SeesawMotion::DefineExtraOptions(QWidget*)
{

}

void SeesawMotion::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
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
            if (ZT == ZONE_TYPE_LINEAR)
            {
                int led_count = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;

                double percent = (Progress/100)*led_count;

                for (int LedID = 0; LedID < led_count; LedID++)
                {
                    Controllers[ControllerID].Controller->SetLED((SetLEDIndex+LedID),
                        percent >= LedID - (width/2) && percent < LedID  ?  UserColors[Dir?0:1] :
                        percent >= LedID && percent < LedID + (width/2)  ?  UserColors[Dir?1:0] :
                                                                              OFF
                    );
                }
            }

            else if (ZT == ZONE_TYPE_MATRIX)
            {
                int cols = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                int rows = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                double percent = (Progress/100)*cols;

                for (int col_id = 0; col_id < cols; col_id++)
                {

                    RGBColor color = percent >= col_id - (width/2)  && percent < col_id ?  UserColors[Dir?0:1] :
                                     percent >= col_id && percent < col_id + (width/2)  ?  UserColors[Dir?1:0] :
                                                                                             OFF;

                    for (int row_id = 0; row_id < rows; row_id++)
                    {
                        int LedID = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->map[((row_id * cols) + col_id)];
                        Controllers[ControllerID].Controller->SetLED(SetLEDIndex + LedID, color);
                    }
                }
            }

        }
    }

    if(Dir)
    {
        if(Progress < 100 + width)
        {
            Progress += float(float(Speed) / float(FPS));
        }
        if(Progress >= 100 + width)
        {
            Dir = false;
            Progress -= float(float(Speed) / float(FPS));
        }
    }

    else
    {
        if(Progress > 0 - 2 * width)
        {
            Progress -= float(float(Speed) / float(FPS));
        }
        if(Progress <= 0 - 2 * width)
        {
            Dir = true;
            Progress += float(float(Speed) / float(FPS));
        }
    }

}

void SeesawMotion::SetSpeed(int NewSpeed)
{
    Speed = NewSpeed;
}

void SeesawMotion::SetUserColors(std::vector<RGBColor> NewUserColors)
{
    UserColors = NewUserColors;
}

void SeesawMotion::Slider2Changed(int NewWidth)
{
    width = NewWidth;
}

void SeesawMotion::ASelectionWasChanged()
{

}
