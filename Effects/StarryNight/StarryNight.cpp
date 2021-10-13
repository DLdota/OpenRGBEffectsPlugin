#include "StarryNight.h"
#include "hsv.h"
#include "ColorUtils.h"

// Note: when assigning to a lot of zones, the last zones in the list do not get a lot of stars.
// This need to be reviewed

REGISTER_EFFECT(StarryNight);

StarryNight::StarryNight() : RGBEffect()
{
    EffectDetails.EffectName = "Starry Night";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Selects a random LED and fades it in an out";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed = 100;
    EffectDetails.MinSpeed = 1;
    EffectDetails.UserColors = 5;

    EffectDetails.MaxSlider2Val = 50;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Star Count";

    EffectDetails.HasCustomWidgets = false;
    EffectDetails.HasCustomSettings = false;

    SetSpeed(50);
    SetSlider2Val(20);
}

void StarryNight::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    int LEDPerCycle = Slider2Val;

    int AmountMadeThisCycle = 0;

    for (unsigned int ControllerID = 0; ControllerID < controller_zones.size(); ControllerID++)
    {
        // Note: this rand() call is the cause, see comment at the top of the file
        // CurrentStars should be a map <ControllerZone*, std::vector<NewStar>>
        // also NewStar should be renamed to "Star" or something more explicit.
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
                    int StartingLED = controller_zones[ControllerID]->start_idx();
                    int RandomLedID = rand() % controller_zones[ControllerID]->leds_count();

                    NewStar LEDStar;
                    LEDStar.Index = ControllerID;
                    LEDStar.LED = (StartingLED + RandomLedID);
                    LEDStar.state = 255;

                    if (RandomColorsEnabled)
                    {
                        LEDStar.Color = ColorUtils::RandomRGBColor();
                    }
                    else
                    {
                        LEDStar.Color = UserColors[rand() % UserColors.size()];
                    }

                    CurrentStars.push_back(LEDStar);
                    AmountMadeThisCycle += 1;
                }
            }

        }
    }

    std::vector<int> ToBeDeleted;

    for (int StarIndex = 0; StarIndex < (int)CurrentStars.size(); StarIndex++)
    {
        unsigned int CTRLR = CurrentStars[StarIndex].Index;

        if(CTRLR < controller_zones.size())
        {
            hsv_t SetColor;
            rgb2hsv(CurrentStars[StarIndex].Color,&SetColor);

            float NewValue = (CurrentStars[StarIndex].state - ((float)Speed / (float)(1000/(float)FPS)));

            if ((NewValue < 1) || (NewValue > 255))
            {
                ToBeDeleted.push_back(StarIndex);
                SetColor.value = 0;
                controller_zones[CTRLR]->SetLED(CurrentStars[StarIndex].LED,hsv2rgb(&SetColor), Brightness);
            }
            else
            {
                SetColor.value = CurrentStars[StarIndex].state;
                controller_zones[CTRLR]->SetLED(CurrentStars[StarIndex].LED,hsv2rgb(&SetColor), Brightness);
                CurrentStars[StarIndex].state -= (float(Speed) / float(FPS) );
            }
        }
        else
        {
             ToBeDeleted.push_back(StarIndex);
        }
    }

    // Go from the front back so that the index doesn't change
    for (int ToDeleteIndex = int(ToBeDeleted.size() - 1); ToDeleteIndex >= 0; ToDeleteIndex--)
    {
        CurrentStars.erase(CurrentStars.begin() + ToBeDeleted[ToDeleteIndex]);
    }
}


void StarryNight::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    CurrentStars.clear();

    for(ControllerZone* controller_zone : controller_zones)
    {
        controller_zone->SetAllZoneLEDs(ColorUtils::OFF(), Brightness);
    }
}
