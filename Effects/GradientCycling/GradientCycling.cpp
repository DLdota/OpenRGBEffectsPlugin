#include "GradientCycling.h"
#include "OpenRGBEffectTab.h"

std::vector<OwnedControllerAndZones> GradientCycling::PrevControllers;

EffectInfo GradientCycling::DefineEffectDetails()
{
    GradientCycling::EffectDetails.EffectName = "Gradient Cycling";
    GradientCycling::EffectDetails.EffectDescription = "Similar to rainbow wave but with 2 colors";

    GradientCycling::EffectDetails.IsReversable  = true;
    GradientCycling::EffectDetails.MaxSpeed      = 30;
    GradientCycling::EffectDetails.MinSpeed      = 1;
    GradientCycling::EffectDetails.UserColors    = 2;

    GradientCycling::EffectDetails.MinSlider2Val = 0;
    GradientCycling::EffectDetails.MaxSlider2Val = 0;
    GradientCycling::EffectDetails.Slider2Name   = "";

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
        if (int(Progress.size()) < (ControllerID + 1))
        {
            std::vector<float> EmptyVector;
            Progress.push_back(EmptyVector);
        }

        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            if (int(Progress[ControllerID].size()) < (ZoneID + 1))
            {
                Progress[ControllerID].push_back(0);
            }

            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;
            int StartIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;
            int LEDCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;
            bool RVRS = OpenRGBEffectTab::CheckReversed(ControllerID, Controllers[ControllerID].OwnedZones[ZoneID]);

            if (ZT == ZONE_TYPE_SINGLE)
            {
                for (int LedID = 0; LedID < LEDCount; LedID++)
                {
                    float GetGradientPos;
                    if (Progress[ControllerID][ZoneID] > FPS) GetGradientPos = (FPS - (Progress[ControllerID][ZoneID] - FPS));
                    else GetGradientPos = Progress[ControllerID][ZoneID];

                    if (GetGradientPos <= 0)
                    {
                        GetGradientPos *= -1;
                    }

                    int RGBCol[3];
                    for (int CVal = 0; CVal < 3; CVal++)
                    {
                        RGBCol[CVal] = int(S[CVal] + (float(GetGradientPos)/float(FPS))*(F[CVal]-S[CVal]));
                    }
                    Controllers[ControllerID].Controller->SetLED((RVRS ? (LEDCount - 1) - LedID : StartIndex + LedID), ToRGBColor(RGBCol[0],RGBCol[1],RGBCol[2]));
                }

                if (Progress[ControllerID][ZoneID] < FPS*2) Progress[ControllerID][ZoneID] = Progress[ControllerID][ZoneID] + ((float)Speed/(float)FPS);
                else if (Progress[ControllerID][ZoneID] >= FPS*2) Progress[ControllerID][ZoneID] = 0;
            }

            else if (ZT == ZONE_TYPE_LINEAR)
            {
                for (int LedID = 0; LedID < LEDCount; LedID++)
                {
                    float GetGradientPos;
                    if ((Progress[ControllerID][ZoneID] + LedID) > LEDCount) GetGradientPos = (LEDCount - ( (Progress[ControllerID][ZoneID] + LedID) - LEDCount));
                    else GetGradientPos = (Progress[ControllerID][ZoneID] + LedID);

                    if (GetGradientPos <= 0)
                    {
                        GetGradientPos *= -1;
                    }

                    int RGBCol[3];
                    for (int CVal = 0; CVal < 3; CVal++)
                    {
                        RGBCol[CVal] = int(S[CVal] + (float(GetGradientPos)/float(LEDCount))*(F[CVal]-S[CVal]));
                    }
                    Controllers[ControllerID].Controller->SetLED((RVRS ? (LEDCount - 1) - LedID : StartIndex + LedID), ToRGBColor(RGBCol[0],RGBCol[1],RGBCol[2]));
                }

                if (Progress[ControllerID][ZoneID] < (LEDCount*2)) Progress[ControllerID][ZoneID] += ((float)Speed / (float)FPS);
                else if (Progress[ControllerID][ZoneID] >= (LEDCount*2)) Progress[ControllerID][ZoneID] = 0;
            }

            else if (ZT == ZONE_TYPE_MATRIX)
            {
                int CollumnCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                int RowCount     = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                for (int CollumnID = 0; CollumnID < CollumnCount; CollumnID++)
                {
                    float GetGradientPos;
                    if ((Progress[ControllerID][ZoneID] + CollumnID) > CollumnCount) GetGradientPos = (CollumnCount - ( (Progress[ControllerID][ZoneID] + CollumnID) - CollumnCount));
                    else GetGradientPos = (Progress[ControllerID][ZoneID] + CollumnID);

                    if (GetGradientPos <= 0)
                    {
                        GetGradientPos *= -1;
                    }

                    int RGBCol[3];
                    for (int CVal = 0; CVal < 3; CVal++)
                    {
                        RGBCol[CVal] = int(S[CVal] + (float(GetGradientPos)/float(CollumnCount))*(F[CVal]-S[CVal]));
                    }

                    for (int RowID = 0; RowID < RowCount; RowID++)
                    {
                        int LedID = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->map[((RowID * CollumnCount) + (RVRS ? (CollumnCount - 1) - CollumnID: CollumnID ) )];
                        Controllers[ControllerID].Controller->SetLED(StartIndex + LedID,ToRGBColor(RGBCol[0],RGBCol[1],RGBCol[2]));
                    }
                }

                if (Progress[ControllerID][ZoneID] < (CollumnCount*2)) Progress[ControllerID][ZoneID] += ((float)Speed / (float)FPS);
                else if (Progress[ControllerID][ZoneID] >= (CollumnCount*2)) Progress[ControllerID][ZoneID] = 0;
            }

        }
    }
}

void GradientCycling::SetSpeed(int Speed)
{
    GradientCycling::Speed = Speed;
}

void GradientCycling::SetUserColors(std::vector<RGBColor> NewColors)
{
    GradientCycling::UserColors = NewColors;
}

void GradientCycling::Slider2Changed(int NewWidth)
{
    GradientCycling::Width = NewWidth;
}

void GradientCycling::ASelectionWasChanged()
{
    Progress.clear();
}
