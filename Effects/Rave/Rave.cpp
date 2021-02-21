#include "Rave.h"

EffectInfo Rave::DefineEffectDetails()
{
    Rave::EffectDetails.EffectName = "Rave";
    Rave::EffectDetails.EffectDescription = "Like rain but with a lot of colors at once";

    Rave::EffectDetails.IsReversable = true;
    Rave::EffectDetails.MaxSpeed = 25;
    Rave::EffectDetails.MinSpeed = 1;
    Rave::EffectDetails.UserColors = 4;

    Rave::EffectDetails.MaxSlider2Val = 20;
    Rave::EffectDetails.MinSlider2Val = 1;
    Rave::EffectDetails.Slider2Name   = "Waves";

    Rave::EffectDetails.HasCustomWidgets = false;
    Rave::EffectDetails.HasCustomSettings = false;

    HasEffect = {{}};
    return Rave::EffectDetails;
}

void Rave::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    if (Controllers.size() != HasEffect[0].size()) this->ASelectionWasChanged({});
    for (int ColorID = 0; ColorID < (int)UserColors.size(); ColorID++)
    {
        for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
        {
            if ((int)CurrentLights.size() >= LightCount) break;
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
                        if ((int)CurrentLights.size() < LightCount)
                        {
                            if (rand()%2 && !HasEffect[ColorID][ControllerID][ActualZone])
                            {
                                LightList NewLight;
                                NewLight.ControllerIndex = ControllerID;
                                NewLight.ZoneIndex       = Controllers[ControllerID].OwnedZones[ZoneID];
                                NewLight.ColorIndex = ColorID;

                                NewLight.LEDCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;
                                NewLight.StartingLED = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;

                                NewLight.ZT = ZONE_TYPE_LINEAR;
                                NewLight.Reversed = OpenRGBEffectTab::CheckReversed(ControllerID,Controllers[ControllerID].OwnedZones[ZoneID]);

                                if (NewLight.Reversed) NewLight.Progress = NewLight.LEDCount;
                                else NewLight.Progress = 0;

                                if (RandomColors)
                                {
                                    NewLight.C = ToRGBColor(rand() % 255,
                                                           rand() % 255,
                                                           rand() % 255);
                                }
                                else
                                {
                                    NewLight.C = UserColors[ColorID];
                                }

                                HasEffect[ColorID][ControllerID][ActualZone] = true;
                                CurrentLights.push_back(NewLight);
                            }
                        }
                    }

                    else if (ZT == ZONE_TYPE_MATRIX)
                    {
                        int MaxAllowed = (rand() % LightCount - (int)CurrentLights.size());
                        int AmountCreated = 0;
                        for (int Created = 0; Created < MaxAllowed; Created++)
                        {
                            int CreateDrop = rand()%(int)Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                            if (AmountCreated < MaxAllowed)
                            {
                                AmountCreated += 1;
                                LightList NewLight;

                                NewLight.ControllerIndex = ControllerID;
                                NewLight.ZoneIndex       = Controllers[ControllerID].OwnedZones[ZoneID];
                                NewLight.ColorIndex = ColorID;

                                NewLight.Collumn = CreateDrop;
                                NewLight.CollumnCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                                NewLight.LEDCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                                if (RandomColors)
                                {
                                    NewLight.C = ToRGBColor(rand() % 255,
                                                           rand() % 255,
                                                           rand() % 255);
                                }
                                else
                                {
                                    NewLight.C = UserColors[ColorID];
                                }

                                NewLight.Progress = 0;
                                NewLight.ZT = ZONE_TYPE_MATRIX;
                                NewLight.Reversed = OpenRGBEffectTab::CheckReversed(ControllerID,Controllers[ControllerID].OwnedZones[ZoneID]);

                                CurrentLights.push_back(NewLight);
                            }
                            else break;
                        }
                    }
                }
            }
        }
    }

    std::vector<int> ToDelete = {};

    for (int LightID = 0; LightID < (int)CurrentLights.size(); LightID++)
    {
        int CIndex = CurrentLights[LightID].ControllerIndex;
        int ZIndex = CurrentLights[LightID].ZoneIndex;
        int SIndex = Controllers[CIndex].Controller->zones[ZIndex].start_idx;

        // Ok so this is kinda hacky.
        RGBColor UserColor = CurrentLights[LightID].C;

        int LedID = CurrentLights[LightID].Progress; // Convert to int so that I can have a solid LED index
        int integral = LedID; // Copy it to another variable so as to not mess stuff up

        if (CurrentLights[LightID].Progress < integral) integral -= 1;
        float fractional = (CurrentLights[LightID].Progress - integral);

        hsv_t FromUserColor;
        rgb2hsv(UserColor, &FromUserColor);

        hsv_t StartingHSV = FromUserColor;
        hsv_t EndingHSV = FromUserColor;

        StartingHSV.value = StartingHSV.value * fractional;
        EndingHSV.value   = EndingHSV.value * -fractional;

        if (CurrentLights[LightID].ZT == ZONE_TYPE_LINEAR)
        {
            // If the drop is reversed
            if (CurrentLights[LightID].Reversed)
            {
                    if (LedID < ( CurrentLights[LightID].LEDCount - 3) ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + (LedID + 3)),ToRGBColor(0,0,0));
                    if (LedID < ( CurrentLights[LightID].LEDCount - 2) ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + (LedID + 2)),hsv2rgb(&StartingHSV));
                    if (LedID < ( CurrentLights[LightID].LEDCount - 1) ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + (LedID + 1)),UserColor);
                    if (LedID < ( CurrentLights[LightID].LEDCount    ) ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + LedID),hsv2rgb(&EndingHSV));
            }
            else
            {
                if (CurrentLights[LightID].Progress < CurrentLights[LightID].LEDCount + 3)
                {
                    if (LedID > 2 ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 3)),ToRGBColor(0,0,0));

                    if (CurrentLights[LightID].Progress < CurrentLights[LightID].LEDCount + 2)
                    {
                        if (LedID > 1 ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 2)),hsv2rgb(&EndingHSV));

                        if (CurrentLights[LightID].Progress < CurrentLights[LightID].LEDCount + 1)
                        {
                            if (LedID > 0 ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 1)),UserColor);
                            if (CurrentLights[LightID].Progress < CurrentLights[LightID].LEDCount)
                            {
                                Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + LedID),hsv2rgb(&StartingHSV));
                            }
                        }
                    }
                }
            }
        }

        else if (CurrentLights[LightID].ZT == ZONE_TYPE_MATRIX)
        {

            int RowID = LedID;
            int CollumnID = CurrentLights[LightID].Collumn;
            int CollumnCount = CurrentLights[LightID].CollumnCount;
            int Led;

            // If the drop is reversed
            if (CurrentLights[LightID].Reversed)
            {
                if (LedID < ( CurrentLights[LightID].LEDCount - 3) )
                {
                    Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * (RowID  + 3) ) + CollumnID)];
                    //Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnID * CurrentLights[LightID].LEDCount) + RowID + 3)];
                    Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + Led),ToRGBColor(0,0,0));
                }

                if (LedID < ( CurrentLights[LightID].LEDCount - 2) )
                {
                    Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * (RowID  + 2) ) + CollumnID)];
                    //Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnID * CurrentLights[LightID].LEDCount) + RowID + 2)];
                    Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&StartingHSV));
                }

                if (LedID < ( CurrentLights[LightID].LEDCount - 1) )
                {
                    Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * (RowID  + 1) ) + CollumnID)];
                    //Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnID * CurrentLights[LightID].LEDCount) + RowID + 1)];
                    Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + Led),UserColor);
                }

                if (LedID < ( CurrentLights[LightID].LEDCount    ) )
                {
                    Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnCount * RowID ) + CollumnID)];
                    //Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnID * CurrentLights[LightID].LEDCount) + RowID)];
                    Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&EndingHSV));
                }
            }
            else
            {
                if (CurrentLights[LightID].Progress < CurrentLights[LightID].LEDCount + 3)
                {
                    Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( ( (RowID - 3) * CollumnCount ) + CollumnID)];
                    //Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnID * CurrentLights[LightID].LEDCount) + RowID - 3)];
                    if (LedID > 2 ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + Led),ToRGBColor(0,0,0));

                    if (CurrentLights[LightID].Progress < CurrentLights[LightID].LEDCount + 2)
                    {
                        Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( ( (RowID - 2) * CollumnCount ) + CollumnID)];
                        //Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnID * CurrentLights[LightID].LEDCount) + RowID - 2)];
                        if (LedID > 1 ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&EndingHSV));

                        if (CurrentLights[LightID].Progress < CurrentLights[LightID].LEDCount + 1)
                        {
                            Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( ( (RowID - 1) * CollumnCount ) + CollumnID)];
                            //Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnID * CurrentLights[LightID].LEDCount) + RowID - 1)];
                            if (LedID > 0 ) Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + Led),UserColor);

                            if (CurrentLights[LightID].Progress < CurrentLights[LightID].LEDCount)
                            {
                                Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (RowID * CollumnCount) + CollumnID)];
                                //Led = Controllers[CIndex].Controller->zones[ZIndex].matrix_map->map[( (CollumnID * CurrentLights[LightID].LEDCount) + RowID)];
                                Controllers[CurrentLights[LightID].ControllerIndex].Controller->SetLED((SIndex + Led),hsv2rgb(&StartingHSV));
                            }
                        }
                    }
                }
            }
        }


        if (CurrentLights[LightID].Reversed)
        {
            if ( (3 + CurrentLights[LightID].Progress) > 0) CurrentLights[LightID].Progress = CurrentLights[LightID].Progress - Speed / (float)FPS;
            else ToDelete.push_back(LightID);
        }
        else
        {
            if (CurrentLights[LightID].Progress < CurrentLights[LightID].LEDCount + 3) CurrentLights[LightID].Progress = CurrentLights[LightID].Progress + Speed / (float)FPS;
            else ToDelete.push_back(LightID);
        }
    }

    if (ToDelete.size() == 0) return;
    for (int DelIndex = ((int)ToDelete.size() - 1) ; DelIndex >= 0; DelIndex--)
    {
        int DIndex = CurrentLights[ToDelete[DelIndex]].ControllerIndex;
        int ZIndex = CurrentLights[ToDelete[DelIndex]].ZoneIndex;
        int CIndex = CurrentLights[ToDelete[DelIndex]].ColorIndex;
        HasEffect[CIndex][DIndex][ZIndex] = false;
        CurrentLights.erase(CurrentLights.begin() + ToDelete[DelIndex]);
    }

    return;
}

void Rave::SetSpeed(int speed)
{
    Speed = speed;
}

void Rave::SetUserColors(std::vector<RGBColor> NewUserColors)
{
    UserColors = NewUserColors;
}

void Rave::Slider2Changed(int NewLightCount)
{
    LightCount = NewLightCount;
}

void Rave::ASelectionWasChanged(std::vector<OwnedControllerAndZones>)
{
    CurrentLights.clear();
    HasEffect.clear();
    for (int ColorID = 0; ColorID < this->EffectDetails.UserColors; ColorID++)
    {
        HasEffect.push_back({});
        for (int ControllerID = 0; ControllerID < (int)ORGBPlugin::RMPointer->GetRGBControllers().size(); ControllerID++)
        {
            HasEffect[ColorID].push_back({});
            for (int ZoneID = 0; ZoneID < (int)ORGBPlugin::RMPointer->GetRGBControllers()[ControllerID]->zones.size(); ZoneID++)
            {
                HasEffect[ColorID][ControllerID].push_back(false);
            }
        }
    }
    CurrentLights.clear();
}

void Rave::ToggleRandomColors(bool RandomEnabled)
{
    RandomColors = RandomEnabled;
}
