#include "StarryNight.h"

std::vector<NewStar> StarryNight::CurrentStars = {};

EffectInfo StarryNight::DefineEffectDetails()
{
    StarryNight::EffectDetails.EffectName = "Starry Night";
    StarryNight::EffectDetails.EffectDescription = "Selects a random LED and fades it in an out";

    StarryNight::EffectDetails.IsReversable = false;
    StarryNight::EffectDetails.MaxSpeed = 100;
    StarryNight::EffectDetails.MinSpeed = 20;
    StarryNight::EffectDetails.UserColors = 5;

    StarryNight::EffectDetails.MaxSlider2Val = 20;
    StarryNight::EffectDetails.MinSlider2Val = 5;
    StarryNight::EffectDetails.Slider2Name   = "Star Count";

    StarryNight::EffectDetails.HasCustomWidgets = false;
    StarryNight::EffectDetails.HasCustomSettings = false;

    return StarryNight::EffectDetails;
}

void StarryNight::DefineExtraOptions(QLayout*){}

void StarryNight::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    if (LEDPerCycle != TempLEDPerCycle)
    {
        LEDPerCycle = TempLEDPerCycle;
    }
    int AmountMadeThisCycle = 0;
    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        if (rand() % 2)
        {
            for (int ZoneID = 0; ZoneID < (int)Controllers[ControllerID].OwnedZones.size(); ZoneID++)
            {
                if (rand() % 2)
                {
                    int MakeForZone = 0;
                    if ((LEDPerCycle - (int(CurrentStars.size()))) > 0)
                    {
                        MakeForZone = rand() % (LEDPerCycle - (int(CurrentStars.size())));
                    }
                    if ((int(CurrentStars.size()) < LEDPerCycle))
                    {
                        for (int ZonesMade = 0; ZonesMade < MakeForZone; ZonesMade++)
                        {
                            int RandRangeMin = Controllers[ControllerID].Controller->zones[ZoneID].start_idx;
                            int RandRangeMax = RandRangeMin + Controllers[ControllerID].Controller->zones[ZoneID].leds_count;

                            int RandomLedID = rand() % RandRangeMax + RandRangeMin;

                            NewStar LEDStar;
                            LEDStar.ControllerIndex = ControllerID;
                            LEDStar.LED = RandomLedID;
                            LEDStar.state = 255;
                            if (RandomColors)
                            {
                                LEDStar.Color = ToRGBColor(
                                            rand() % 255, /* R */
                                            rand() % 255, /* G */
                                            rand() % 255  /* B */
                                            );
                            }
                            else
                            {
                                LEDStar.Color = UserColors[rand() % 4];
                            }

                            CurrentStars.push_back(LEDStar);
                            AmountMadeThisCycle += 1;
                        }
                    }
                }
            }
        }
    }

    std::vector<int> ToBeDeleted;
    for (int StarIndex = 0; StarIndex < (int)CurrentStars.size(); StarIndex++)
    {
        /*-------*\
        | Setup   |
        \*-------*/
        int CTRLR = CurrentStars[StarIndex].ControllerIndex;
        hsv_t SetColor;
        rgb2hsv(CurrentStars[StarIndex].Color,&SetColor);


        float NewValue = (CurrentStars[StarIndex].state - ( (float)Speed / (float)(1000/(float)FPS) ) );
        if ((NewValue < 1) || (NewValue > 255))
        {
            ToBeDeleted.push_back(StarIndex);
            SetColor.value = 0;
            Controllers[CTRLR].Controller->SetLED(CurrentStars[StarIndex].LED,hsv2rgb(&SetColor));
        }
        else
        {
            SetColor.value = CurrentStars[StarIndex].state;
            Controllers[CTRLR].Controller->SetLED(CurrentStars[StarIndex].LED,hsv2rgb(&SetColor));
            CurrentStars[StarIndex].state -= (float(Speed) / float(FPS) );
        }
    }

    // Go from the front back so that the index doesn't change
    for (int ToDeleteIndex = int(ToBeDeleted.size() - 1); ToDeleteIndex >= 0; ToDeleteIndex--)
    {
        CurrentStars.erase(CurrentStars.begin() + ToBeDeleted[ToDeleteIndex]);
    }

    return;
}

void StarryNight::SetSpeed(int Speed)
{
    StarryNight::Speed = Speed;
}

void StarryNight::SetUserColors(std::vector<RGBColor> NewColors)
{
    StarryNight::UserColors = NewColors;
}

void StarryNight::Slider2Changed(int LEDCount)
{
    StarryNight::TempLEDPerCycle = LEDCount;
}

void StarryNight::ASelectionWasChanged()
{
    CurrentStars.clear();
}

void StarryNight::ToggleRandomColors(bool RandomEnabled)
{
    RandomColors = RandomEnabled;
}
