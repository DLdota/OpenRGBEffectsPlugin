#include "Visor.h"
#include "OpenRGBEffectTab.h"

RGBColor OFF = ToRGBColor(0,0,0);

EffectInfo Visor::DefineEffectDetails()
{
    Visor::EffectDetails.EffectName = "Visor";
    Visor::EffectDetails.EffectDescription = "A back and forth effect motion, flipping colors";

    Visor::EffectDetails.IsReversable = false;
    Visor::EffectDetails.MaxSpeed     = 100;
    Visor::EffectDetails.MinSpeed     = 10;

    Visor::EffectDetails.UserColors   = 2;

    Visor::EffectDetails.MinSlider2Val = 3;
    Visor::EffectDetails.MaxSlider2Val = 50;
    Visor::EffectDetails.Slider2Name   = "Width";

    Visor::EffectDetails.HasCustomWidgets = false;
    Visor::EffectDetails.HasCustomSettings = false;

    return Visor::EffectDetails;
}

void Visor::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{    
    current_head_hue = Dir ? Head.hue: Tail.hue;
    current_tail_hue = Dir ? Tail.hue: Head.hue;

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            /*-------------------*\
            | Setup for the loop  |
            \*-------------------*/
            int SetLEDIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;
            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;

            /*----------------------------------------------------*\
            | Adjust how it applies for the specific type of zone  |
            \*----------------------------------------------------*/
            if (ZT == ZONE_TYPE_LINEAR)
            {
                int led_count = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;

                for (int LedID = 0; LedID < led_count; LedID++)
                {
                    Controllers[ControllerID].Controller->SetLED((SetLEDIndex+LedID), GetColor(LedID, led_count));
                }
            }

            else if (ZT == ZONE_TYPE_MATRIX)
            {
                int cols = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                int rows = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                for (int col_id = 0; col_id < cols; col_id++)
                {
                    RGBColor color = GetColor(col_id, cols);

                    for (int row_id = 0; row_id < rows; row_id++)
                    {
                        int LedID = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->map[((row_id * cols) + col_id)];
                        Controllers[ControllerID].Controller->SetLED(SetLEDIndex + LedID, color);
                    }
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

    if(flipping && Random)
    {
        GenerateRandomColors();
    }

}

void Visor::GenerateRandomColors()
{
    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;
    RGBColor C1 = ToRGBColor(r, g, b);
    RGBColor C2 = ToRGBColor((255-r),(255-g),(255-b));

    rgb2hsv(C1, &Head);
    rgb2hsv(C2, &Tail);
}

void Visor::ToggleRandomColors(bool NewRandom)
{
    Random = NewRandom;

    if(Random)
    {
        GenerateRandomColors();
    }
    else
    {
        SetUserColors(UserColors);
    }
}

void Visor::SetSpeed(int NewSpeed)
{
    Speed = NewSpeed;
}

void Visor::SetUserColors(std::vector<RGBColor> NewUserColors)
{
    UserColors = NewUserColors;

    if(!Random)
    {
        rgb2hsv(UserColors[0], &Head);
        rgb2hsv(UserColors[1], &Tail);
    }
}

void Visor::Slider2Changed(int NewWidth)
{
    width = NewWidth * 2 ;
}

RGBColor Visor::GetColor(int i, int count)
{
    float percent = (Progress/100)* (count+width+1);

    float whole;
    float linear_fractional = std::modf(percent, &whole);
    float linear_neg_fractional = 1.0f - linear_fractional;

    float fractional = pow(linear_fractional,  3.0);
    float neg_fractional = 1.0f - pow(linear_fractional, 1.0/3.0);

    int current_first_led = (int) whole;
    int half_width = width/2;

    float value;
    float hue;
    float sat;

    // black leds
    if(i < current_first_led  - width || i > current_first_led  -1)
    {
        return OFF;
    }

    // tail led
    if(current_first_led  - width== i)
    {
        value = neg_fractional * Tail.value;
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
