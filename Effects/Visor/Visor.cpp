#include "Visor.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Visor);

Visor::Visor() : RGBEffect()
{
    EffectDetails.EffectName = "Visor";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A back and forth effect motion, flipping colors";
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.Slider2Name   = "Width";

    SetSpeed(50);
    SetSlider2Val(20);

    C0 = ColorUtils::RandomRGBColor();
    C1 = ColorUtils::RandomRGBColor();
}

void Visor::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    // Caculate a few things outside the loop
    Progress += 0.01 * float(Speed) / float(FPS);

    width = 0.01 * Slider2Val;          // [0-1] float, size of the visor
    h_width = 0.5 * width;              // [0-0.5] float, half width
    p = (Progress - (long) Progress);   // [0-1] float, 0 to 1 progress
    step = p < 0.5;                     // p in [0-0.5] = fist step, p in [0.5-1] = second step
    p_step =  step ? 2.*p : 2.*(1.- p); // [0-1] float, 0 to 1 progress within the step
    x_step = p_step * (1.+ 4.*width) - 1.5*width;

    bool flipping = (last_step != step);

    if(flipping) last_step = step;

    if(flipping && RandomColorsEnabled)
    {
        C0 = ColorUtils::RandomRGBColor();
        C1 = ColorUtils::RandomRGBColor();
    }
    else if(!RandomColorsEnabled)
    {
        C0 = UserColors[0];
        C1 = UserColors[1];
    }

    for(ControllerZone* controller_zone: controller_zones)
    {
        /*-------------------*\
        | Setup for the loop  |
        \*-------------------*/
        int start_idx = controller_zone->start_idx();
        zone_type ZT = controller_zone->type();

        /*----------------------------------------------------*\
        | Adjust how it applies for the specific type of zone  |
        \*----------------------------------------------------*/
        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            int leds_count = controller_zone->leds_count();

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                controller_zone->SetLED((start_idx + LedID), GetColor(LedID, leds_count), Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                RGBColor color = GetColor(col_id, cols);

                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zone->controller->zones[controller_zone->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zone->SetLED(start_idx + LedID, color, Brightness);
                }
            }
        }
    }

}

RGBColor Visor::GetColor(float i, float count)
{
    // dont divide by zero
    if(count <= 1)
    {
        count = 2;
    }

    float x = i / (count-1);
    float dist = x_step - x;

    // fade the head
    if(dist < 0)
    {
        float l = std::max<float>(0.,std::min<float>((width+dist)/width, 1.));
        return step ? ColorUtils::Enlight(C1, l) : ColorUtils::Enlight(C0, l) ;
    }

    // fade the tail
    if(dist > width)
    {
        float l = std::max<float>(0.,std::min<float>(1.-((dist-width)/width), 1.));
        return step ? ColorUtils::Enlight(C0, l) : ColorUtils::Enlight(C1, l) ;
    }

    // interpolate colors

    float interp = std::min<float>(std::max<float>((width-dist)/width, 0.),1.);

    return step ?
                ColorUtils::Interpolate(C0,C1,interp):
                ColorUtils::Interpolate(C1,C0,interp);

}
