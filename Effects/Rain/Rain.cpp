#include "Rain.h"
#include "ORGBEffectPlugin.h"
#include "EffectManager.h"
#include "hsv.h"

EffectInfo Rain::DefineEffectDetails()
{
    Rain::EffectDetails.EffectName = "Rain";
    Rain::EffectDetails.EffectDescription = "Picks a random zone/collumn and makes a droplet effect";

    Rain::EffectDetails.IsReversable = true;
    Rain::EffectDetails.MaxSpeed = 25;
    Rain::EffectDetails.MinSpeed = 1;

    Rain::EffectDetails.AllowOnlyFirst = true;
    Rain::EffectDetails.UserColors = 5;

    Rain::EffectDetails.MaxSlider2Val = 20;
    Rain::EffectDetails.MinSlider2Val = 1;
    Rain::EffectDetails.Slider2Name   = "Drops";

    Rain::EffectDetails.HasCustomWidgets = false;
    Rain::EffectDetails.HasCustomSettings = false;

    return Rain::EffectDetails;
}

void Rain::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    if (Controllers.size() != HasEffect.size())
    {
        this->ASelectionWasChanged(Controllers);
    }

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        if ((int)CurrentDrops.size() >= Slider2Val)
        {
            break;
        }

        if (rand() % 2)
        {
            for (int ZoneID = 0; ZoneID < (int)Controllers[ControllerID].OwnedZones.size(); ZoneID++)
            {
                zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;
                int ActualZone = Controllers[ControllerID].OwnedZones[ZoneID];

                if (ZT == ZONE_TYPE_SINGLE)
                {
                    // Rain doesn't support single LED zones
                    continue;
                }

                else if (ZT == ZONE_TYPE_LINEAR)
                {
                    if ((int)CurrentDrops.size() < Slider2Val)
                    {
                        if (rand()%2 && !HasEffect[ControllerID][ActualZone])
                        {
                            Drops NewDrop;
                            NewDrop.ControllerIndex = ControllerID;
                            NewDrop.ZoneIndex       = Controllers[ControllerID].OwnedZones[ZoneID];

                            NewDrop.LEDCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;
                            NewDrop.StartingLED = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;

                            NewDrop.ZT = ZONE_TYPE_LINEAR;
                            NewDrop.Reversed = EffectManager::Get()->CheckReversed(ControllerID,Controllers[ControllerID].OwnedZones[ZoneID]);

                            NewDrop.Progress = 0;

                            if (RandomColorsEnabled)
                            {
                                NewDrop.C = ToRGBColor(rand() % 255,
                                                       rand() % 255,
                                                       rand() % 255);
                            }
                            else
                            {
                                if (OnlyFirstColorEnabled)
                                {
                                    NewDrop.C = UserColors[0];
                                }
                                else
                                {
                                    NewDrop.C = UserColors[rand()%this->EffectDetails.UserColors];
                                }
                            }

                            HasEffect[ControllerID][ActualZone] = true;
                            CurrentDrops.push_back(NewDrop);
                        }
                    }
                }

                else if (ZT == ZONE_TYPE_MATRIX)
                {
                    int MaxAllowed = (rand() % (int)Slider2Val - (int)CurrentDrops.size());
                    int AmountCreated = 0;

                    for (int Created = 0; Created < MaxAllowed; Created++)
                    {
                        int CreateDrop = rand()%(int)Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;

                        if (AmountCreated < MaxAllowed)
                        {
                            AmountCreated += 1;
                            Drops NewDrop;

                            NewDrop.ControllerIndex = ControllerID;
                            NewDrop.ZoneIndex       = Controllers[ControllerID].OwnedZones[ZoneID];

                            NewDrop.Collumn = CreateDrop;
                            NewDrop.CollumnCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                            NewDrop.LEDCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                            NewDrop.Progress = 0;
                            NewDrop.ZT = ZONE_TYPE_MATRIX;
                            NewDrop.Reversed = EffectManager::Get()->CheckReversed(ControllerID,Controllers[ControllerID].OwnedZones[ZoneID]);

                            if (RandomColorsEnabled)
                            {
                                NewDrop.C = ToRGBColor(rand() % 255,
                                                       rand() % 255,
                                                       rand() % 255);
                            }
                            else
                            {
                                if (OnlyFirstColorEnabled)
                                {
                                    NewDrop.C = UserColors[0];
                                }
                                else
                                {
                                    NewDrop.C = UserColors[rand()%this->EffectDetails.UserColors];
                                }
                            }

                            CurrentDrops.push_back(NewDrop);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

    std::vector<int> ToDelete = {};

    for (int DropID = 0; DropID < (int)CurrentDrops.size(); DropID++)
    {
        int CIndex = CurrentDrops[DropID].ControllerIndex;
        int ZIndex = CurrentDrops[DropID].ZoneIndex;
        int SIndex = Controllers[CIndex].Controller->zones[ZIndex].start_idx;
        int CCount = CurrentDrops[DropID].LEDCount;

        // Ok so this is kinda hacky.
        RGBColor UserColor = CurrentDrops[DropID].C;

        int LedID = CurrentDrops[DropID].Progress; // Convert to int so that I can have a solid LED index
        int integral = LedID; // Copy it to another variable so as to not mess stuff up

        float fractional;

        if ((int)CurrentDrops[DropID].Progress == 0)
        {
            fractional = CurrentDrops[DropID].Progress;
        }
        else
        {
            fractional = (CurrentDrops[DropID].Progress - integral);
        }

        hsv_t FromUserColor;
        rgb2hsv(UserColor, &FromUserColor);

        hsv_t StartingHSV = FromUserColor;
        hsv_t EndingHSV = FromUserColor;

        if ((signed)fractional > 1)
        {
            fractional = 1;
        }
        else if ((signed)fractional < 0)
        {
            fractional = 0;
        }

        StartingHSV.value = StartingHSV.value * fractional;
        EndingHSV.value   = EndingHSV.value * (1 - fractional);

        if (CurrentDrops[DropID].ZT == ZONE_TYPE_LINEAR)
        {
            // If the drop is reversed
            if (CurrentDrops[DropID].Reversed)
            {
                if (0 < ( (CCount - LedID) + 3) ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + ((CCount - LedID) + 3)), ToRGBColor(0,0,0)     );
                if (0 < ( (CCount - LedID) + 0) ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + ((CCount - LedID) + 0)), hsv2rgb(&StartingHSV));
                if (0 < ( (CCount - LedID) + 1) ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + ((CCount - LedID) + 1)), UserColor            );
                if (0 < ( (CCount - LedID) + 2) ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + ((CCount - LedID) + 2)), hsv2rgb(&EndingHSV)  );
            }
            else
            {
                if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 3)
                {
                    if (LedID > 2)
                    {
                        Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 3)),ToRGBColor(0,0,0));
                    }

                    if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 2)
                    {
                        if (LedID > 1)
                        {
                            Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 2)),hsv2rgb(&EndingHSV));
                        }

                        if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 1)
                        {
                            if (LedID > 0)
                            {
                                Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 1)),UserColor);
                            }

                            if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount)
                            {
                                Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + LedID),hsv2rgb(&StartingHSV));
                            }
                        }
                    }
                }
            }
        }

        else if (CurrentDrops[DropID].ZT == ZONE_TYPE_MATRIX)
        {
            {
                int RowID = LedID;
                int CollumnID = CurrentDrops[DropID].Collumn;
                int CollumnCount = CurrentDrops[DropID].CollumnCount;
                int Led;

                // If the drop is reversed
                if (CurrentDrops[DropID].Reversed)
                {
                    if (0 < ( (CurrentDrops[DropID].LEDCount - RowID) + 3) )
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * ((CurrentDrops[DropID].LEDCount - RowID) + 3)) + CollumnID)];
                        Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),ToRGBColor(0,0,0));
                    }

                    if (0 < ( (CurrentDrops[DropID].LEDCount - RowID) + 2) )
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * ((CurrentDrops[DropID].LEDCount - RowID) + 2 )) + CollumnID)];
                        Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&EndingHSV));
                    }

                    if (0 < ( (CurrentDrops[DropID].LEDCount - RowID) + 1) )
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * ((CurrentDrops[DropID].LEDCount - RowID)  + 1)) + CollumnID)];
                        Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),UserColor);
                    }

                    if (0 < ( (CurrentDrops[DropID].LEDCount - RowID) + 0) )
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * ((CurrentDrops[DropID].LEDCount - RowID)  + 0)) + CollumnID)];
                        Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&StartingHSV));
                    }

                }
                else
                {
                    if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 3)
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( ( (RowID - 3) * CollumnCount ) + CollumnID)];

                        if (LedID > 2 )
                        {
                            Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),ToRGBColor(0,0,0));
                        }

                        if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 2)
                        {
                            Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( ( (RowID - 2) * CollumnCount ) + CollumnID)];

                            if (LedID > 1 )
                            {
                                Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&EndingHSV));
                            }

                            if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 1)
                            {
                                Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( ( (RowID - 1) * CollumnCount ) + CollumnID)];

                                if (LedID > 0 )
                                {
                                    Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),UserColor);
                                }

                                if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount)
                                {
                                    Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (RowID * CollumnCount) + CollumnID)];
                                    Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&StartingHSV));
                                }
                            }
                        }
                    }
                }
            }
        }

        if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 3)
        {
            CurrentDrops[DropID].Progress = CurrentDrops[DropID].Progress + Speed / (float)FPS;
        }

        else
        {
            ToDelete.push_back(DropID);
        }
    }

    if (ToDelete.size() == 0)
    {
        return;
    }

    for (int DelIndex = ((int)ToDelete.size() - 1) ; DelIndex >= 0; DelIndex--)
    {
        int CIndex = CurrentDrops[ToDelete[DelIndex]].ControllerIndex;
        int ZIndex = CurrentDrops[ToDelete[DelIndex]].ZoneIndex;
        HasEffect[CIndex][ZIndex] = false;

        CurrentDrops.erase(CurrentDrops.begin() + ToDelete[DelIndex]);
    }

}

/*-----------------------*\
| Setters for Effect GUI  |
\*-----------------------*/
void Rain::ASelectionWasChanged(std::vector<OwnedControllerAndZones>)
{
    HasEffect.clear();

    for (int ControllerID = 0; ControllerID < (int)ORGBPlugin::RMPointer->GetRGBControllers().size(); ControllerID++)
    {
        HasEffect.push_back({});

        for (int ZoneID = 0; ZoneID < (int)ORGBPlugin::RMPointer->GetRGBControllers()[ControllerID]->zones.size(); ZoneID++)
        {
            HasEffect[ControllerID].push_back(false);
        }
    }

    CurrentDrops.clear();
}
