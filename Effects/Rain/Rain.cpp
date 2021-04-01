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
    Rain::EffectDetails.UserColors = 1;

    Rain::EffectDetails.MaxSlider2Val = 20;
    Rain::EffectDetails.MinSlider2Val = 1;
    Rain::EffectDetails.Slider2Name   = "Drops";

    Rain::EffectDetails.HasCustomWidgets = false;
    Rain::EffectDetails.HasCustomSettings = false;

    return Rain::EffectDetails;
}

void Rain::DefineExtraOptions(QLayout*){}

void Rain::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    if (Controllers.size() != HasEffect.size()) this->ASelectionWasChanged(Controllers);
    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        if ((int)CurrentDrops.size() >= DropCount) break;
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
                    if ((int)CurrentDrops.size() < DropCount)
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

                            if (NewDrop.Reversed) NewDrop.Progress = NewDrop.LEDCount;
                            else NewDrop.Progress = 0;

                            if (RandomColors)
                            {
                                NewDrop.C = ToRGBColor(rand() % 255,
                                                       rand() % 255,
                                                       rand() % 255);
                            }
                            else
                            {
                                NewDrop.C = UserColor;
                            }

                            HasEffect[ControllerID][ActualZone] = true;
                            CurrentDrops.push_back(NewDrop);
                        }
                    }
                }

                else if (ZT == ZONE_TYPE_MATRIX)
                {
                    int MaxAllowed = (rand() % DropCount - (int)CurrentDrops.size());
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

                            if (RandomColors)
                            {
                                NewDrop.C = ToRGBColor(rand() % 255,
                                                       rand() % 255,
                                                       rand() % 255);
                            }
                            else
                            {
                                NewDrop.C = UserColor;
                            }

                            CurrentDrops.push_back(NewDrop);
                        }
                        else break;
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

        // Ok so this is kinda hacky.
        RGBColor UserColor = CurrentDrops[DropID].C;



        int LedID = CurrentDrops[DropID].Progress; // Convert to int so that I can have a solid LED index
        int integral = LedID; // Copy it to another variable so as to not mess stuff up

        float fractional;

        if ((int)CurrentDrops[DropID].Progress == 0) fractional = CurrentDrops[DropID].Progress;
        else fractional = (CurrentDrops[DropID].Progress - integral);

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

        if (SpeedSliderChanged)
        {
            if (LedID > PrevNum)
            {
                CurrentDrops[DropID].Progress = LedID;
                SpeedSliderChanged = false;
            }
            else
            {
                PrevNum = LedID;
            }
        }

        StartingHSV.value = StartingHSV.value * fractional;
        EndingHSV.value   = EndingHSV.value * -fractional;

        if (CurrentDrops[DropID].ZT == ZONE_TYPE_LINEAR)
        {
            // If the drop is reversed
            if (CurrentDrops[DropID].Reversed)
            {
                if (LedID < ( CurrentDrops[DropID].LEDCount - 3) ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID + 3)),ToRGBColor(0,0,0));
                if (LedID < ( CurrentDrops[DropID].LEDCount - 2) ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID + 2)),hsv2rgb(&StartingHSV));
                if (LedID < ( CurrentDrops[DropID].LEDCount - 1) ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID + 1)),UserColor);
                if (LedID < ( CurrentDrops[DropID].LEDCount    ) ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + LedID),hsv2rgb(&EndingHSV));
            }
            else
            {
                if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 3)
                {
                    if (LedID > 2 ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 3)),ToRGBColor(0,0,0));

                    if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 2)
                    {
                        if (LedID > 1 ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 2)),hsv2rgb(&EndingHSV));

                        if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 1)
                        {
                            if (LedID > 0 ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 1)),UserColor);
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
                    if (LedID < ( CurrentDrops[DropID].LEDCount - 3) )
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * (RowID  + 3) ) + CollumnID)];
                        Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),ToRGBColor(0,0,0));
                    }

                    if (LedID < ( CurrentDrops[DropID].LEDCount - 2) )
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * (RowID  + 2) ) + CollumnID)];
                        Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&StartingHSV));
                    }

                    if (LedID < ( CurrentDrops[DropID].LEDCount - 1) )
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * (RowID  + 1) ) + CollumnID)];
                        Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),UserColor);
                    }

                    if (LedID < ( CurrentDrops[DropID].LEDCount    ) )
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * RowID ) + CollumnID)];
                        Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&EndingHSV));
                    }
                }
                else
                {
                    if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 3)
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( ( (RowID - 3) * CollumnCount ) + CollumnID)];
                        if (LedID > 2 ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),ToRGBColor(0,0,0));

                        if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 2)
                        {
                            Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( ( (RowID - 2) * CollumnCount ) + CollumnID)];
                            if (LedID > 1 ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&EndingHSV));

                            if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 1)
                            {
                                Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( ( (RowID - 1) * CollumnCount ) + CollumnID)];
                                if (LedID > 0 ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + Led),UserColor);

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


        if (CurrentDrops[DropID].Reversed)
        {
            if ( (3 + CurrentDrops[DropID].Progress) > 0) CurrentDrops[DropID].Progress = CurrentDrops[DropID].Progress - Speed / (float)FPS;
            else ToDelete.push_back(DropID);
        }
        else
        {
            if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 3) CurrentDrops[DropID].Progress = CurrentDrops[DropID].Progress + Speed / (float)FPS;
            else ToDelete.push_back(DropID);
        }
    }

    if (ToDelete.size() == 0) return;
    for (int DelIndex = ((int)ToDelete.size() - 1) ; DelIndex >= 0; DelIndex--)
    {
        int CIndex = CurrentDrops[ToDelete[DelIndex]].ControllerIndex;
        int ZIndex = CurrentDrops[ToDelete[DelIndex]].ZoneIndex;
        HasEffect[CIndex][ZIndex] = false;
        CurrentDrops.erase(CurrentDrops.begin() + ToDelete[DelIndex]);
    }

    return;
}


/*-----------------------*\
| Setters for Effect GUI  |
\*-----------------------*/
void Rain::SetSpeed(int NewSpeed)
{
    Rain::Speed = NewSpeed;
    SpeedSliderChanged = true;
}

void Rain::SetUserColors(std::vector<RGBColor> NewColor)
{
    Rain::UserColor = NewColor[0];
}

void Rain::Slider2Changed(int NewDropCount)
{
    Rain::DropCount = NewDropCount;
}

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


/*--------------------------*\
| Getters for the Effect GUI |
\*--------------------------*/
int Rain::GetSpeed(){ return Rain::Speed; }

int Rain::GetSlider2Val(){ return Rain::DropCount; }

std::vector<RGBColor> Rain::GetUserColors(){ return {Rain::UserColor}; }

void Rain::ToggleRandomColors(bool RandomEnabled)
{
    RandomColors = RandomEnabled;
}
