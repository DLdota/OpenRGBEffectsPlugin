#include "Breathing.h"

EffectInfo Breathing::DefineEffectDetails()
{
    Breathing::EffectDetails.EffectName = "Breathing";
    Breathing::EffectDetails.EffectDescription = "Fading in and out 1 color across an entire zone";

    Breathing::EffectDetails.IsReversable = false;
    Breathing::EffectDetails.MaxSpeed     = 200;
    Breathing::EffectDetails.MinSpeed     = 30;
    Breathing::EffectDetails.UserColors   = 1;

    Breathing::EffectDetails.MaxSlider2Val = 0;
    Breathing::EffectDetails.MinSlider2Val = 0;
    Breathing::EffectDetails.Slider2Name   = "";

    Breathing::EffectDetails.HasCustomWidgets = false;
    Breathing::EffectDetails.HasCustomSettings = false;

    return Breathing::EffectDetails;
}

void Breathing::DefineExtraOptions(QLayout*){}

void Breathing::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    hsv_t HSVVal;
    if (RandomThisCycle)
    {
        if (RandomColors && !Dir && !AlreadyMade)
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

    if (HSVVal.value > 0)
    {
        if (Dir)
        {
            if (Progress > HSVVal.value)
            {
                if (RandomColors)
                {
                    RandomThisCycle = true;
                }
                else
                {
                    RandomThisCycle = false;
                }
                AlreadyMade = false;
                Dir = !Dir;
                Progress = HSVVal.value;
                HSVVal.value = 0;
            }
            else {HSVVal.value = HSVVal.value - Progress; Progress = Progress + (float(Speed) / float(FPS));};
        }
        else
        {
            if ((Progress <= 0) || (Progress > 256) /* If it goes over 255 */ )
            {
                Dir = !Dir;
                Progress = 0;
            }
            else
            {
                HSVVal.value = HSVVal.value - Progress;
                Progress = Progress - (float(Speed) / float(FPS));
            }
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
