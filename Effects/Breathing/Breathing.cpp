#include "Breathing.h"

EffectInfo Breathing::DefineEffectDetails()
{
    Breathing::EffectDetails.EffectName = "Breathing";
    Breathing::EffectDetails.EffectDescription = "Fading in and out 1 color across an entire zone";

    Breathing::EffectDetails.IsReversable = false;
    Breathing::EffectDetails.MaxSpeed     = 60;
    Breathing::EffectDetails.MinSpeed     = 1;
    Breathing::EffectDetails.UserColors   = 1;

    Breathing::EffectDetails.MaxSlider2Val = 0;
    Breathing::EffectDetails.MinSlider2Val = 0;
    Breathing::EffectDetails.Slider2Name   = "";

    return Breathing::EffectDetails;
}

void Breathing::DefineExtraOptions(QWidget *Parent)
{

}

void Breathing::StepEffect(std::vector<OwnedControllerAndZones> LocalControllers, int FPS)
{

}

void Breathing::SetSpeed(int Speed)
{

}

void Breathing::SetUserColors(std::vector<RGBColor> NewUserColors)
{
    UserColors = NewUserColors;
}

void Breathing::Slider2Changed(int)
{

}

void Breathing::ASelectionWasChanged()
{

}
