#include "Breathing.h"
#include "hsv.h"

EffectInfo Breathing::DefineEffectDetails()
{
    Breathing::EffectDetails.EffectName = "Breathing";
    Breathing::EffectDetails.EffectDescription = "Fading in and out 1 color across an entire zone";

    Breathing::EffectDetails.IsReversable = false;
    Breathing::EffectDetails.MaxSpeed     = 200;
    Breathing::EffectDetails.MinSpeed     = 40;
    Breathing::EffectDetails.UserColors   = 1;

    Breathing::EffectDetails.MaxSlider2Val = 0;
    Breathing::EffectDetails.MinSlider2Val = 0;
    Breathing::EffectDetails.Slider2Name   = "";

    Breathing::EffectDetails.HasCustomWidgets = false;
    Breathing::EffectDetails.HasCustomSettings = false;

    return Breathing::EffectDetails;
}

void Breathing::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{   
    Progress += ((Speed / 100.0) / (float)FPS);

    if(Progress >= 3.14159) // PI
    {
        Progress -= 3.14159;

        if(RandomColors)
        {
           rgb2hsv(ToRGBColor(rand() % 255, rand() % 255, rand() % 255), &CurrentColor);
        }
        else
        {
           rgb2hsv(UserColors[0], &CurrentColor);
        }
    }

    CurrentColor.value = pow(sin(Progress),3) * 255;

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            Controllers[ControllerID].Controller->SetAllZoneLEDs(Controllers[ControllerID].OwnedZones[ZoneID],hsv2rgb(&CurrentColor));
        }
    }
}

void Breathing::SetSpeed(int NewSpeed)
{
    Speed = NewSpeed;
}

void Breathing::SetUserColors(std::vector<RGBColor> NewUserColors)
{
    UserColors = NewUserColors;
    rgb2hsv(UserColors[0], &CurrentColor);
}

void Breathing::ToggleRandomColors(bool RandomEnabled)
{
    RandomColors = RandomEnabled;
}
