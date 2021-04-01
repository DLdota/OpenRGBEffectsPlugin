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
    hsv_t HSVVal;
    if (RandomThisCycle)
    {
        if (RandomColors && GoingUp && !AlreadyMade)
        {
            RandomColor = ToRGBColor(rand() % 255, rand() % 255, rand() % 255);
            rgb2hsv(RandomColor, &HSVVal);
            if (HSVVal.value < Progress)
            {
                Progress = HSVVal.value;
            }
            AlreadyMade = true;
        }
        else
        {
            rgb2hsv(RandomColor,&HSVVal);
        }
    }
    else
    {
        rgb2hsv(UserColors[0],&HSVVal);
    }

    if (GoingUp)
    {
        if (Progress > 1)
        {
            GoingUp = false;
            Progress = 1;
        }
        else
        {
            HSVVal.value = HSVVal.value * Progress;
            Progress += ((Speed / 100.0) / (float)FPS);
        }
    }
    else
    {
        if (Progress < 0)
        {
            GoingUp = true;
            if (RandomColors){RandomThisCycle = true;}
            else{RandomThisCycle = false;}
            AlreadyMade = false;
            Progress = 0;
            HSVVal.value = 0;
        }
        else
        {
            HSVVal.value = HSVVal.value * Progress;
            Progress -= ((Speed / 100.0) / (float)FPS);
        }
    }

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            Controllers[ControllerID].Controller->SetAllZoneLEDs(Controllers[ControllerID].OwnedZones[ZoneID],hsv2rgb(&HSVVal));
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
}

void Breathing::ToggleRandomColors(bool RandomEnabled)
{
    RandomColors = RandomEnabled;
}
