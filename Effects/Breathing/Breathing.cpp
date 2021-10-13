#include "Breathing.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Breathing);

Breathing::Breathing() : RGBEffect()
{
    EffectDetails.EffectName = "Breathing";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Fading in and out 1 color across an entire zone";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 10;
    EffectDetails.UserColors   = 1;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = false;
    EffectDetails.HasCustomSettings = false;

    SetSpeed(100);
}

void Breathing::StepEffect(std::vector<ControllerZone*> controller_zones)
{   
    Progress += ((Speed / 100.0) / (float)FPS);

    if(Progress >= 3.14159) // PI
    {
        Progress -= 3.14159;

        if(RandomColorsEnabled)
        {
           CurrentColor = ColorUtils::RandomHSVColor();
        }
        else
        {
           rgb2hsv(UserColors[0], &CurrentColor);
        }
    }

    CurrentColor.value = pow(sin(Progress),3) * 255;

    for (ControllerZone* controller_zone: controller_zones)
    {
        controller_zone->SetAllZoneLEDs(hsv2rgb(&CurrentColor), Brightness);
    }
}


void Breathing::SetUserColors(std::vector<RGBColor> NewUserColors)
{
    UserColors = NewUserColors;
    rgb2hsv(UserColors[0], &CurrentColor);
}

