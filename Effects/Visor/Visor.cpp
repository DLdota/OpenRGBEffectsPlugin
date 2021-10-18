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
    EffectDetails.MinSlider2Val = 3;
    EffectDetails.MaxSlider2Val = 50;
    EffectDetails.Slider2Name   = "Width";

    SetSpeed(50);
}

void Visor::StepEffect(std::vector<ControllerZone*> controller_zones)
{    
    current_head_hue = Dir ? Head.hue: Tail.hue;
    current_tail_hue = Dir ? Tail.hue: Head.hue;

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

    bool flipping = false;

    if(Dir)
    {
        if(Progress < 100)
        {
            Progress += float(float(Speed) / float(FPS));
        }
        if(Progress >= 100)
        {
            Dir = false;
            Progress -= float(float(Speed) / float(FPS));
            flipping = true;
        }
    }

    else
    {
        if(Progress > 0)
        {
            Progress -= float(float(Speed) / float(FPS));
        }
        if(Progress <= 0)
        {
            Dir = true;
            Progress += float(float(Speed) / float(FPS));
            flipping = true;
        }
    }

    if(flipping && RandomColorsEnabled)
    {
        GenerateRandomColors();
    }

}

void Visor::GenerateRandomColors()
{
    RGBColor C1 = ColorUtils::RandomRGBColor();
    RGBColor C2 = ColorUtils::Invert(C1);

    rgb2hsv(C1, &Head);
    rgb2hsv(C2, &Tail);
}

void Visor::SetRandomColorsEnabled(bool value)
{
    RandomColorsEnabled = value;

    if(RandomColorsEnabled)
    {
        GenerateRandomColors();
    }
    else
    {
        SetUserColors(UserColors);
    }
}

void Visor::SetUserColors(std::vector<RGBColor> NewUserColors)
{
    UserColors = NewUserColors;

    if(!RandomColorsEnabled)
    {
        rgb2hsv(UserColors[0], &Head);
        rgb2hsv(UserColors[1], &Tail);
    }
}

void Visor::SetSlider2Val(unsigned int value)
{
    width = value * 2 ;
}

unsigned int Visor::GetSlider2Val()
{
    return width / 2;
}

RGBColor Visor::GetColor(int i, int count)
{
    float percent = (Progress/100)* (count+width+1);

    float whole;
    float linear_fractional = std::modf(percent, &whole);
    float linear_neg_fractional = 1.0f - linear_fractional;
    float fractional = pow(linear_fractional,  3.0);

    int current_first_led = (int) whole;
    int half_width = width/2;

    float value = 0;
    float hue = 0;
    float sat = 0;

    // black leds
    if(i < current_first_led  - width || i > current_first_led  -1)
    {        
        return ColorUtils::OFF();
    }

    // tail led
    if(current_first_led  - width== i)
    {
        value = linear_neg_fractional * Tail.value;
        sat = Tail.saturation;
        hue = current_tail_hue;
    }
    // other tail leds
    else if(i <= current_first_led - half_width - 2)
    {
        value = Tail.value;
        sat = Tail.saturation;
        hue = current_tail_hue;
    }
    // first led of tail
    else if(i == current_first_led - half_width - 1)
    {
        value = Tail.value;
        sat = linear_fractional * Tail.saturation;
        hue = current_tail_hue;
    }
    // last led of head
    else if(i == current_first_led - width + half_width)
    {
        value = Head.value;
        sat = linear_neg_fractional * Head.saturation;
        hue = current_head_hue;
    }
    // head leds
    else if (i < current_first_led  - 1)
    {
        value = Head.value;
        sat = Head.saturation;
        hue = current_head_hue;
    }
    // first led of head
    else if(i == current_first_led - 1)
    {
        value = fractional * Head.value;
        sat = Head.saturation;
        hue = current_head_hue;
    }

    hsv_t HSVal;

    HSVal.hue = hue;
    HSVal.saturation = sat;
    HSVal.value = value;

    return RGBColor(hsv2rgb(&HSVal));
}
