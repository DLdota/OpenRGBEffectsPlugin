#include "Rain.h"
#include "hsv.h"

REGISTER_EFFECT(Rain);

Rain::Rain() : RGBEffect()
{
    EffectDetails.EffectName = "Rain";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Picks a random zone/column and makes a droplet effect";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed = 25;
    EffectDetails.MinSpeed = 1;

    EffectDetails.AllowOnlyFirst = true;
    EffectDetails.UserColors = 5;

    EffectDetails.MaxSlider2Val = 20;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Drops";

    EffectDetails.HasCustomWidgets = false;
    EffectDetails.HasCustomSettings = false;
}

void Rain::StepEffect(std::vector<ControllerZone> controller_zones)
{
    if (controller_zones.size() != HasEffect.size())
    {
        ASelectionWasChanged(controller_zones);
    }

    for (int ControllerID = 0; ControllerID < int(controller_zones.size()); ControllerID++)
    {
        if (CurrentDrops.size() >= Slider2Val)
        {
            break;
        }

        if (rand() % 2)
        {

            zone_type ZT =  controller_zones[ControllerID].type();

            if (ZT == ZONE_TYPE_SINGLE)
            {
                // Rain doesn't support single LED zones
                continue;
            }

            else if (ZT == ZONE_TYPE_LINEAR)
            {
                if (CurrentDrops.size() < Slider2Val)
                {
                    if (rand()%2 && !HasEffect[ControllerID])
                    {
                        Drops NewDrop;
                        NewDrop.Index = ControllerID;

                        NewDrop.LEDCount = controller_zones[ControllerID].leds_count();
                        NewDrop.StartingLED = controller_zones[ControllerID].start_idx();


                        NewDrop.ZT = ZONE_TYPE_LINEAR;
                        NewDrop.Reversed = controller_zones[ControllerID].reverse;

                        NewDrop.Progress = 0;

                        if (RandomColorsEnabled)
                        {
                            NewDrop.C = ToRGBColor(rand() % 255,
                                                   rand() % 255,
                                                   rand() % 255);
                        }
                        else if (OnlyFirstColorEnabled)
                        {
                            NewDrop.C = UserColors[0];
                        }
                        else
                        {
                            NewDrop.C = UserColors[rand()%this->EffectDetails.UserColors];
                        }

                        HasEffect[ControllerID] = true;
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
                    int CreateDrop = rand()%(int)controller_zones[ControllerID].matrix_map_width();

                    if (AmountCreated < MaxAllowed)
                    {
                        AmountCreated += 1;
                        Drops NewDrop;

                        NewDrop.Index = ControllerID;

                        NewDrop.Column = CreateDrop;
                        NewDrop.ColumnCount = controller_zones[ControllerID].matrix_map_width();
                        NewDrop.LEDCount = controller_zones[ControllerID].matrix_map_height();

                        NewDrop.Progress = 0;
                        NewDrop.ZT = ZONE_TYPE_MATRIX;
                        NewDrop.Reversed = controller_zones[ControllerID].reverse;

                        if (RandomColorsEnabled)
                        {
                            NewDrop.C = ToRGBColor(rand() % 255,
                                                   rand() % 255,
                                                   rand() % 255);
                        }
                        else if (OnlyFirstColorEnabled)
                        {
                            NewDrop.C = UserColors[0];
                        }
                        else
                        {
                            NewDrop.C = UserColors[rand()%this->EffectDetails.UserColors];
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

    std::vector<int> ToDelete = {};

    for (int DropID = 0; DropID < (int)CurrentDrops.size(); DropID++)
    {
        int CIndex = CurrentDrops[DropID].Index;
        int SIndex = controller_zones[CIndex].start_idx();
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
                if (0 <= ( (CCount - LedID) + 3) ) controller_zones[CIndex].controller->SetLED((SIndex + ((CCount - LedID) + 3)), ToRGBColor(0,0,0)     );
                if (0 <= ( (CCount - LedID) + 0) ) controller_zones[CIndex].controller->SetLED((SIndex + ((CCount - LedID) + 0)), hsv2rgb(&StartingHSV));
                if (0 <= ( (CCount - LedID) + 1) ) controller_zones[CIndex].controller->SetLED((SIndex + ((CCount - LedID) + 1)), UserColor            );
                if (0 <= ( (CCount - LedID) + 2) ) controller_zones[CIndex].controller->SetLED((SIndex + ((CCount - LedID) + 2)), hsv2rgb(&EndingHSV)  );
            }
            else
            {
                if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 3)
                {
                    if (LedID > 2)
                    {
                        controller_zones[CIndex].controller->SetLED((SIndex + (LedID - 3)),ToRGBColor(0,0,0));
                    }

                    if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 2)
                    {
                        if (LedID > 1)
                        {
                            controller_zones[CIndex].controller->SetLED((SIndex + (LedID - 2)),hsv2rgb(&EndingHSV));
                        }

                        if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 1)
                        {
                            if (LedID > 0)
                            {
                                controller_zones[CIndex].controller->SetLED((SIndex + (LedID - 1)),UserColor);
                            }

                            if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount)
                            {
                                controller_zones[CIndex].controller->SetLED((SIndex + LedID),hsv2rgb(&StartingHSV));
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
                int ColumnID = CurrentDrops[DropID].Column;
                int ColumnCount = CurrentDrops[DropID].ColumnCount;
                int Led;

                int ZIndex = controller_zones[CIndex].zone_idx;

                // If the drop is reversed
                if (CurrentDrops[DropID].Reversed)
                {
                    if (0 < ( (CurrentDrops[DropID].LEDCount - RowID) + 3) )
                    {
                        Led = controller_zones[CIndex].controller->zones[ZIndex].matrix_map->map[( (ColumnCount * ((CurrentDrops[DropID].LEDCount - RowID) + 3)) + ColumnID)];
                        controller_zones[CIndex].controller->SetLED((SIndex + Led),ToRGBColor(0,0,0));
                    }

                    if (0 < ( (CurrentDrops[DropID].LEDCount - RowID) + 2) )
                    {
                        Led = controller_zones[CIndex].controller->zones[ZIndex].matrix_map->map[( (ColumnCount * ((CurrentDrops[DropID].LEDCount - RowID) + 2 )) + ColumnID)];
                        controller_zones[CIndex].controller->SetLED((SIndex + Led),hsv2rgb(&EndingHSV));
                    }

                    if (0 < ( (CurrentDrops[DropID].LEDCount - RowID) + 1) )
                    {
                        Led = controller_zones[CIndex].controller->zones[ZIndex].matrix_map->map[( (ColumnCount * ((CurrentDrops[DropID].LEDCount - RowID)  + 1)) + ColumnID)];
                        controller_zones[CIndex].controller->SetLED((SIndex + Led),UserColor);
                    }

                    if (0 < ( (CurrentDrops[DropID].LEDCount - RowID) + 0) )
                    {
                        Led = controller_zones[CIndex].controller->zones[ZIndex].matrix_map->map[( (ColumnCount * ((CurrentDrops[DropID].LEDCount - RowID)  + 0)) + ColumnID)];
                        controller_zones[CIndex].controller->SetLED((SIndex + Led),hsv2rgb(&StartingHSV));
                    }

                }
                else
                {
                    if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 3)
                    {
                        Led = controller_zones[CIndex].controller->zones[ZIndex].matrix_map->map[( ( (RowID - 3) * ColumnCount ) + ColumnID)];

                        if (LedID > 2 )
                        {
                            controller_zones[CIndex].controller->SetLED((SIndex + Led),ToRGBColor(0,0,0));
                        }

                        if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 2)
                        {
                            Led = controller_zones[CIndex].controller->zones[ZIndex].matrix_map->map[( ( (RowID - 2) * ColumnCount ) + ColumnID)];

                            if (LedID > 1 )
                            {
                                controller_zones[CIndex].controller->SetLED((SIndex + Led),hsv2rgb(&EndingHSV));
                            }

                            if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount + 1)
                            {
                                Led = controller_zones[CIndex].controller->zones[ZIndex].matrix_map->map[( ( (RowID - 1) * ColumnCount ) + ColumnID)];

                                if (LedID > 0 )
                                {
                                    controller_zones[CIndex].controller->SetLED((SIndex + Led),UserColor);
                                }

                                if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount)
                                {
                                    Led = controller_zones[CIndex].controller->zones[ZIndex].matrix_map->map[( (RowID * ColumnCount) + ColumnID)];
                                    controller_zones[CIndex].controller->SetLED((SIndex + Led),hsv2rgb(&StartingHSV));
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
        int CIndex = CurrentDrops[ToDelete[DelIndex]].Index;
        HasEffect[CIndex] = false;

        CurrentDrops.erase(CurrentDrops.begin() + ToDelete[DelIndex]);
    }

}

/*-----------------------*\
| Setters for Effect GUI  |
\*-----------------------*/
void Rain::ASelectionWasChanged(std::vector<ControllerZone> controller_zones)
{
    RGBColor OFF = ToRGBColor(0,0,0);

    HasEffect.clear();
    CurrentDrops.clear();

    for(ControllerZone controller_zone: controller_zones)
    {
        HasEffect.push_back(false);
        controller_zone.controller->SetAllZoneLEDs(controller_zone.zone_idx, OFF);
    }
}
