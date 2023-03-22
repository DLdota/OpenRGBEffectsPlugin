#include "RainbowWave.h"
#include "hsv.h"

REGISTER_EFFECT(RainbowWave);

RainbowWave::RainbowWave() : RGBEffect()
{
    EffectDetails.EffectName = "Rainbow Wave";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A sliding Rainbow";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.MaxSlider2Val = 50;
    EffectDetails.Slider2Name   = "Frequency";
    EffectDetails.SupportsRandom = false;

    SetSpeed(40);
    SetSlider2Val(10);
}

void RainbowWave::StepEffect(std::vector<ControllerZone*> controller_zones)
{    
    int Width = Slider2Val;

    hsv_t HSVVal;
    HSVVal.saturation = 255;
    HSVVal.value      = 255;

    for(ControllerZone* controller_zone: controller_zones)
    {
        /*-------------------*\
        | Setup for the loop  |
        \*-------------------*/
        int leds_count = controller_zone->leds_count();
        zone_type ZT = controller_zone->type();
        bool RVRS = controller_zone->reverse;

        /*----------------------------------------------------*\
        | Adjust how it applies for the specific type of zone  |
        \*----------------------------------------------------*/
        if (ZT == ZONE_TYPE_SINGLE)
        {
            int HUE = (Progress * Width);
            HSVVal.hue = HUE;
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                controller_zone->SetLED(LedID, RGBColor(hsv2rgb(&HSVVal)), Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                int HUE;
                if (RVRS) HUE = ((Progress + ( (leds_count - 1) - LedID) ) * Width);
                else HUE = ((Progress + LedID) * Width);

                HSVVal.hue = HUE;

                controller_zone->SetLED(LedID, RGBColor(hsv2rgb(&HSVVal)), Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int ColumnCount = controller_zone->matrix_map_width();
            int RowCount = controller_zone->matrix_map_height();

            for (int ColumnID = 0; ColumnID < ColumnCount; ColumnID++)
            {
                int HUE;

                if (RVRS)
                {
                    HUE = ((Progress + (int)( (ColumnCount - 1) - ColumnID)) * Width);
                }
                else
                {
                    HUE = ((Progress + (int)ColumnID) * Width);
                }

                HSVVal.hue = HUE;

                for (int RowID = 0; RowID < RowCount; RowID++)
                {
                    int LedID = controller_zone->controller->zones[controller_zone->zone_idx].matrix_map->map[((RowID * ColumnCount) + ColumnID)];
                    controller_zone->SetLED(LedID, RGBColor(hsv2rgb(&HSVVal)), Brightness);
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
}

