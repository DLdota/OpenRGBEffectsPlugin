#include "GradientCycling.h"
#include "OpenRGBEffectTab.h"

EffectInfo GradientCycling::DefineEffectDetails()
{
    GradientCycling::EffectDetails.EffectName = "Gradient Cycling";
    GradientCycling::EffectDetails.EffectDescription = "Similar to rainbow wave but with 2 colors";

    GradientCycling::EffectDetails.IsReversable  = true;
    GradientCycling::EffectDetails.MaxSpeed      = 30;
    GradientCycling::EffectDetails.MinSpeed      = 1;
    GradientCycling::EffectDetails.UserColors    = 2;

    GradientCycling::EffectDetails.MinSlider2Val = 5;
    GradientCycling::EffectDetails.MaxSlider2Val = 50;
    GradientCycling::EffectDetails.Slider2Name   = "Width";

    hsv_t BlankColor;
    BlankColor.hue = 0;
    BlankColor.saturation = 255;
    BlankColor.value = 255;

    StartingColor = BlankColor;
    EndingColor = BlankColor;

    return GradientCycling::EffectDetails;
}

void GradientCycling::DefineExtraOptions(QWidget *Parent)
{
    return;
}

void GradientCycling::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    int S[3];
    S[0] = (UserColors[0] & 0x00ff0000) >> 16;
    S[1] = (UserColors[0] & 0x0000ff00) >> 8;
    S[2] = (UserColors[0] & 0x000000ff);

    int F[3];
    F[0] = (UserColors[1] & 0x00ff0000) >> 16;
    F[1] = (UserColors[1] & 0x0000ff00) >> 8;
    F[2] = (UserColors[1] & 0x000000ff);

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;
            int StartIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;
            int LEDCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;
            bool RVRS = OpenRGBEffectTab::CheckReversed(ControllerID, Controllers[ControllerID].OwnedZones[ZoneID]);

            if (ZT == ZONE_TYPE_LINEAR)
            {
                for (int LedID = 0; LedID < LEDCount; LedID++)
                {
                    int RGBCol[3];
                    for (int CVal = 0; CVal < 3; CVal++)
                    {
                        float GetGradientPos;
                        if ((Progress + LedID) > LEDCount) GetGradientPos = (LEDCount - ( (Progress + LedID) - LEDCount));
                        else GetGradientPos = (Progress + LedID);

                        RGBCol[CVal] = int(S[CVal] + (float(GetGradientPos * Width)/float(LEDCount * Width))*(F[CVal]-S[CVal]));
                    }
                    Controllers[ControllerID].Controller->SetLED(StartIndex + LedID, ToRGBColor(RGBCol[0],RGBCol[1],RGBCol[2]));
                }
            }
            else if (ZT == ZONE_TYPE_MATRIX)
            {

            }
            else if (ZT == ZONE_TYPE_SINGLE)
            {

            }
        }
    }
    if (Progress < (Width * 2)) Progress += ((float)Speed / (float)FPS);
    else if (Progress >= (Width * 2)) Progress = 0;
}

void GradientCycling::SetSpeed(int Speed)
{
    GradientCycling::Speed = Speed;
}

void GradientCycling::SetUserColors(std::vector<RGBColor> NewColors)
{
    GradientCycling::UserColors = NewColors;
    hsv_t Comp1;
    hsv_t Comp2;
    rgb2hsv(UserColors[0],&Comp1);
    rgb2hsv(UserColors[1],&Comp2);

    if (Comp1.hue < Comp2.hue)
    {
        EndingColor.hue = Comp2.hue;
        StartingColor.hue = Comp1.hue;
    }
    else if (Comp1.hue > Comp2.hue)
    {
        EndingColor.hue = Comp1.hue;
        StartingColor.hue = Comp2.hue;
    }
    else if (Comp1.hue == Comp2.hue)
    {
        EndingColor.hue = Comp1.hue;
        StartingColor.hue = Comp2.hue + 1;
    }
}

void GradientCycling::Slider2Changed(int NewWidth)
{
    GradientCycling::Width = NewWidth;
}
