#include "Lightning.h"

REGISTER_EFFECT(Lightning);

Lightning::Lightning(QWidget *parent) :
    QWidget(parent),
    RGBEffect()
{
    EffectDetails.EffectName = "Lightning";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Lightning effect";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 50;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 1;

    EffectDetails.MaxSlider2Val = 60;
    EffectDetails.MinSlider2Val = 10;
    EffectDetails.Slider2Name   = "Decay";

    EffectDetails.HasCustomWidgets = false;
    EffectDetails.HasCustomSettings = false;
}

RGBColor Lightning::TriggerLightning(int n)
{
    int Decay = Slider2Val;

    float decrease = 1 + Decay/(float) FPS;

    Lightnings[n].value = ((unsigned int)(rand() % 1000)) > (1000 - Speed) ? std::min<unsigned char>(255, RandomColorsEnabled ? 255: UserHSV.value) :  Lightnings[n].value > 0 ?  Lightnings[n].value / decrease : 0;

    if(RandomColorsEnabled)
    {
        if(Lightnings[n].value == 0)
        {
            Lightnings[n].hue = rand() % 360;
            Lightnings[n].saturation = rand() % 255;
        }
    }
    else
    {
        Lightnings[n].hue = UserHSV.hue;
        Lightnings[n].saturation = UserHSV.saturation;
    }

    return RGBColor(hsv2rgb(&Lightnings[n]));
}

void  Lightning::ASelectionWasChanged(std::vector<ControllerZone> controller_zones)
{
    Lightnings.clear();

    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        hsv_t lightning;

        lightning.value = 0;
        lightning.hue = UserHSV.hue;
        lightning.saturation = UserHSV.saturation;

        Lightnings.push_back(lightning);
    }
}


void Lightning::StepEffect(std::vector<ControllerZone> controller_zones)
{
    if(Lightnings.size() != controller_zones.size())
    {
        ASelectionWasChanged(controller_zones);
    }

    int n = 0;

    for(ControllerZone& controller_zone : controller_zones)
    {
        RGBColor color = TriggerLightning(n++);
        controller_zone.controller->SetAllZoneLEDs(controller_zone.zone_idx, color);
    }
}

void Lightning::SetUserColors(std::vector<RGBColor> colors) {
    UserColors = colors;
    rgb2hsv(UserColors[0], &UserHSV);
}
