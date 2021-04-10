#include "Lightning.h"

/*------------------------------*\
| RGBEffect Standard Functions  |
\*-----------------------------*/
Lightning::Lightning(QWidget *parent) :
    QWidget(parent)
{

}

EffectInfo Lightning::DefineEffectDetails()
{
    Lightning::EffectDetails.EffectName = "Lightning";
    Lightning::EffectDetails.EffectDescription = "Lightning effect";

    Lightning::EffectDetails.IsReversable = false;
    Lightning::EffectDetails.MaxSpeed     = 50;
    Lightning::EffectDetails.MinSpeed     = 1;
    Lightning::EffectDetails.UserColors   = 1;

    Lightning::EffectDetails.MaxSlider2Val = 60;
    Lightning::EffectDetails.MinSlider2Val = 10;
    Lightning::EffectDetails.Slider2Name   = "Decay";

    Lightning::EffectDetails.HasCustomWidgets = false;
    Lightning::EffectDetails.HasCustomSettings = false;

    return Lightning::EffectDetails;
}

void Lightning::SetSpeed(int value)
{
    Speed = value;
}

void Lightning::ToggleRandomColors(bool value)
{
    Random = value;
}

void Lightning::Slider2Changed(int value)
{
    Decay = value;
}

RGBColor Lightning::TriggerLightning(int n, int FPS)
{
    float decrease = 1 + Decay/(float) FPS;

    Lightnings[n].value = ((unsigned int)(rand() % 1000)) > (1000 - Speed) ? std::min<unsigned char>(255, UserHSV.value) :  Lightnings[n].value > 0 ?  Lightnings[n].value / decrease : 0;

    if(Random)
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

void  Lightning::ASelectionWasChanged(std::vector<OwnedControllerAndZones> Controllers)
{
    Lightnings.clear();

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            hsv_t lightning;

            lightning.value = 0;
            lightning.hue = UserHSV.hue;
            lightning.saturation = UserHSV.saturation;

            Lightnings.push_back(lightning);
        }
    }
}


void Lightning::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    int n = 0;

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            RGBColor color = TriggerLightning(n++, FPS);

            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;

            if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
            {
                int SetLEDIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;

                for (int LedID = 0; LedID < int(Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count); LedID++)
                {
                    Controllers[ControllerID].Controller->SetLED((SetLEDIndex + LedID), color);
                }
            }
            else if (ZT == ZONE_TYPE_MATRIX)
            {
                int cols = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                int rows = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                for (int col_id = 0; col_id < cols; col_id++)
                {
                    for (int row_id = 0; row_id < rows; row_id++)
                    {
                        int LedID = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->map[((row_id * cols) + col_id)];
                        Controllers[ControllerID].Controller->SetLED(LedID, color);
                    }
                }
            }

        }
    }
}

void Lightning::SetUserColors(std::vector<RGBColor> colors) {
    UserColors = colors;
    rgb2hsv(UserColors[0], &UserHSV);
}
