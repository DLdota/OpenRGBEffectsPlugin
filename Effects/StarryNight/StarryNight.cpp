#include "StarryNight.h"

EffectInfo StarryNight::DefineEffectDetails()
{
    StarryNight::EffectDetails.EffectName = "Starry Night";
    StarryNight::EffectDetails.EffectDescription = "Selects a random LED and fades it in an out";

    StarryNight::EffectDetails.IsReversable = false;
    StarryNight::EffectDetails.MaxSpeed = 100;
    StarryNight::EffectDetails.MinSpeed = 1;
    StarryNight::EffectDetails.UserColors = 5;

    StarryNight::EffectDetails.MaxSlider2Val = 10;
    StarryNight::EffectDetails.MinSlider2Val = 1;
    StarryNight::EffectDetails.Slider2Name   = "Amount of \"stars\"";

    return StarryNight::EffectDetails;
}

void StarryNight::DefineExtraOptions(QWidget *Parent)
{
    // No extra options
    return;
}

void StarryNight::StepEffect(std::vector<OwnedControllerAndZones>, int FPS)
{
    // No effect structure yet
    return;
}

void StarryNight::SetSpeed(int Speed)
{
    StarryNight::Speed = Speed;
}

void StarryNight::SetUserColors(std::vector<RGBColor> NewColors)
{
    StarryNight::UserColors = NewColors;
}

void StarryNight::Slider2Changed(int LEDCount)
{
    StarryNight::LEDPerCycle = LEDCount;
}
