#include "StarryNight.h"

EffectInfo StarryNight::DefineEffectDetails()
{
    StarryNight::EffectDetails.EffectName = "Starry Night";
    StarryNight::EffectDetails.EffectDescription = "Selects a random LED and fades it in an out";

    StarryNight::EffectDetails.IsReversable = false;
    StarryNight::EffectDetails.MaxSpeed = 100;
    StarryNight::EffectDetails.MinSpeed = 1;
    StarryNight::EffectDetails.UserColors = 5;

    StarryNight::EffectDetails.MaxSlider2Val = 20;
    StarryNight::EffectDetails.MinSlider2Val = 5;
    StarryNight::EffectDetails.Slider2Name   = "Star Count";

    return StarryNight::EffectDetails;
}

void StarryNight::DefineExtraOptions(QWidget *Parent)
{
    // No extra options
    return;
}

void StarryNight::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    return;
    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < (int)Controllers[ControllerID].OwnedZones.size(); ZoneID++)
        {
            if ((int)CurrentStars.size() < LEDPerCycle)
            {
                int MakeStarCount = rand() % CurrentStars.size() + LEDPerCycle;
                for (int StarCountProgress = 0; StarCountProgress < MakeStarCount; StarCountProgress++)
                {
                    //qDebug() << "Creating a new star";
                    int RandRangeMin = Controllers[ControllerID].Controller->zones[ZoneID].start_idx;
                    int RandRangeMax = RandRangeMin + Controllers[ControllerID].Controller->zones[ZoneID].leds_count;

                    int RandomLedID = rand() % RandRangeMax + RandRangeMin;

                    NewStar LEDStar;
                    LEDStar.ControllerIndex = ControllerID;
                    LEDStar.LED = RandomLedID;
                    LEDStar.state = 255;
                    LEDStar.Color = UserColors[rand() % 4];

                    CurrentStars.push_back(LEDStar);
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
        bool ToBeDeleted = false;
        hsv_t SetColor;
        rgb2hsv(CurrentStars[StarIndex].Color,&SetColor);


        float NewValue = (CurrentStars[StarIndex].state - ( Speed / (1000/FPS) ) );
        if ((NewValue < 1) || (NewValue > 255))
        {
            ToBeDeleted = true;
            SetColor.value = 0;
            Controllers[CTRLR].Controller->SetLED(CurrentStars[StarIndex].LED,hsv2rgb(&SetColor));
        }
        else
        {
            SetColor.value = CurrentStars[StarIndex].state;
            Controllers[CTRLR].Controller->SetLED(CurrentStars[StarIndex].LED,hsv2rgb(&SetColor));
            CurrentStars[StarIndex].state = (CurrentStars[StarIndex].state - ( Speed / (1000/FPS) ) );
        }
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
    StarryNight::LEDPerCycle = LEDCount;
}
