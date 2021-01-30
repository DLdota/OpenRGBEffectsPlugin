#include "Rain.h"
#include "OpenRGBEffectTab.h"

#define MAXLEDS 4

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

    return Rain::EffectDetails;
}

void Rain::DefineExtraOptions(QWidget*){/*No Extra Options*/}

void Rain::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        if ((int)CurrentDrops.size() >= DropCount) break;
        if (rand() % 2)
        {
            for (int ZoneID = 0; ZoneID < (int)Controllers[ControllerID].OwnedZones.size(); ZoneID++)
            {
                zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;
                if (ZT == ZONE_TYPE_SINGLE)
                {
                    // Rain won't support single LED zones
                    continue;
                }

                else if (ZT == ZONE_TYPE_LINEAR)
                {
                    if ((int)CurrentDrops.size() < DropCount)
                    {
                        if (rand()%2)
                        {
                            Drops NewDrop;
                            NewDrop.ControllerIndex = ControllerID;
                            NewDrop.ZoneIndex       = Controllers[ControllerID].OwnedZones[ZoneID];

                            NewDrop.LEDCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;
                            NewDrop.StartingLED = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;


                            NewDrop.ZT = ZONE_TYPE_LINEAR;
                            NewDrop.Reversed = OpenRGBEffectTab::CheckReversed(ControllerID,Controllers[ControllerID].OwnedZones[ZoneID]);

                            if (NewDrop.Reversed) NewDrop.Progress = NewDrop.LEDCount;
                            else NewDrop.Progress = 0;

                            CurrentDrops.push_back(NewDrop);
                        }
                    }
                }

                else if (ZT == ZONE_TYPE_MATRIX)
                {
                    int MaxAllowed = (rand() % DropCount - CurrentDrops.size());
                    int AmountCreated = 0;
                    for (int Col = 0; Col < (int)Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width; Col++)
                    {
                        if (AmountCreated < MaxAllowed)
                        {
                            if (rand()%2)
                            {
                                AmountCreated += 1;
                                Drops NewDrop;

                                NewDrop.ControllerIndex = ControllerID;
                                NewDrop.ZoneIndex       = Controllers[ControllerID].OwnedZones[ZoneID];

                                NewDrop.Collumn = Col;
                                NewDrop.LEDCount = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                                NewDrop.Progress = 0;
                                NewDrop.ZT = ZONE_TYPE_MATRIX;
                                NewDrop.Reversed = OpenRGBEffectTab::CheckReversed(ControllerID,Controllers[ControllerID].OwnedZones[ZoneID]);

                                CurrentDrops.push_back(NewDrop);
                            }
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

        int LedID = CurrentDrops[DropID].Progress; // Convert to int so that I can have a solid LED index
        int integral = LedID; // Copy it to another variable so as to not mess stuff up

        if (CurrentDrops[DropID].Progress < integral) integral -= 1;
        float fractional = (CurrentDrops[DropID].Progress - integral);

        //float* fractionalPTR = &fractional; // Convert to pointer so I can index it

        char array[10];
        sprintf(array, "%f", fractional);

        if (array[2] == 0)
        {
            if (CurrentDrops[DropID].Reversed)
            {
                if (CurrentDrops[DropID].Progress > 0) CurrentDrops[DropID].Progress = CurrentDrops[DropID].Progress - Speed / (float)FPS;
                else {ToDelete.push_back(DropID); Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + LedID),ToRGBColor(0,0,0));}
            }
            else
            {
                if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount) CurrentDrops[DropID].Progress = CurrentDrops[DropID].Progress + Speed / (float)FPS;
                else {ToDelete.push_back(DropID); Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + LedID),ToRGBColor(0,0,0));}
            }
            continue;
        }

        qDebug() << array[2];

        hsv_t FromUserColor;
        rgb2hsv(UserColor, &FromUserColor);

        hsv_t StartingHSV = FromUserColor;
        hsv_t EndingHSV = FromUserColor;

        StartingHSV.value = StartingHSV.value * fractional;
        EndingHSV.value   = EndingHSV.value / fractional;

        if (CurrentDrops[DropID].ZT == ZONE_TYPE_LINEAR)
        {
            if (CurrentDrops[DropID].Reversed)
            {
                //qDebug() << EndingHSV.value;
                //if (LedID > 0) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID - 1)),hsv2rgb(&EndingHSV));
                if (LedID < ( CurrentDrops[DropID].LEDCount - 1) ) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + (LedID + 1)),ToRGBColor(0,0,0));
                Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + LedID),hsv2rgb(&EndingHSV));
            }
            else
            {
                //if (LedID < CurrentDrops[DropID].LEDCount) Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + LedID),ToRGBColor(0,0,0));
                //Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + LedID),hsv2rgb(&StartingHSV));
            }
        }

        else if (CurrentDrops[DropID].ZT == ZONE_TYPE_MATRIX)
        {

        }


        if (CurrentDrops[DropID].Reversed)
        {
            if (CurrentDrops[DropID].Progress > 0) CurrentDrops[DropID].Progress = CurrentDrops[DropID].Progress - Speed / (float)FPS;
            else {ToDelete.push_back(DropID); Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + LedID),ToRGBColor(0,0,0));}
        }
        else
        {
            if (CurrentDrops[DropID].Progress < CurrentDrops[DropID].LEDCount) CurrentDrops[DropID].Progress = CurrentDrops[DropID].Progress + Speed / (float)FPS;
            else {ToDelete.push_back(DropID); Controllers[CurrentDrops[DropID].ControllerIndex].Controller->SetLED((SIndex + LedID),ToRGBColor(0,0,0));}
        }
    }

    if (ToDelete.size() == 0) return;
    for (int DelIndex = (ToDelete.size() - 1) ; DelIndex >= 0; DelIndex--)
    {
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
}

void Rain::SetUserColors(std::vector<RGBColor> NewColor)
{
    Rain::UserColor = NewColor[0];
}

void Rain::Slider2Changed(int NewDropCount)
{
    Rain::DropCount = NewDropCount;
}

void Rain::ASelectionWasChanged()
{
    CurrentDrops.clear();
}


/*--------------------------*\
| Getters for the Effect GUI |
\*--------------------------*/
int Rain::GetSpeed(){ return Rain::Speed; }

int Rain::GetSlider2Val(){ return Rain::DropCount; }

std::vector<RGBColor> Rain::GetUserColors(){ return {Rain::UserColor}; }
