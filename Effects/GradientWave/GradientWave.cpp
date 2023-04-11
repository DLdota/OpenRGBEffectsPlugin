#include "GradientWave.h"
#include "ColorUtils.h"

REGISTER_EFFECT(GradientWave);

GradientWave::GradientWave() : RGBEffect()
{
    EffectDetails.EffectName = "Gradient Wave";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Similar to rainbow wave but with 2 colors";
    EffectDetails.IsReversable  = true;
    EffectDetails.MaxSpeed      = 30;
    EffectDetails.MinSpeed      = 1;
    EffectDetails.UserColors    = 2;

    SetSpeed(10);
}

void GradientWave::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    int F[3];
    int S[3];

    if (RandomColorsEnabled)
    {
        S[0] = RGBGetRValue(RandomColorList[0]);
        S[1] = RGBGetGValue(RandomColorList[0]);
        S[2] = RGBGetBValue(RandomColorList[0]);

        F[0] = RGBGetRValue(RandomColorList[1]);
        F[1] = RGBGetGValue(RandomColorList[1]);
        F[2] = RGBGetBValue(RandomColorList[1]);
    }
    else
    {
        S[0] = RGBGetRValue(UserColors[0]);
        S[1] = RGBGetGValue(UserColors[0]);
        S[2] = RGBGetBValue(UserColors[0]);

        F[0] = RGBGetRValue(UserColors[1]);
        F[1] = RGBGetGValue(UserColors[1]);
        F[2] = RGBGetBValue(UserColors[1]);
    }

    if(Progress.size() != controller_zones.size())
    {
        Progress.clear();
        Progress.resize(controller_zones.size());
    }

    int i = 0;

    for (ControllerZone* controller_zone: controller_zones)
    {
        zone_type ZT = controller_zone->type();
        int LEDCount = controller_zone->leds_count();
        bool RVRS = controller_zone->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < LEDCount; LedID++)
            {
                float GetGradientPos;

                if ((Progress[i] + LedID) > LEDCount)
                {
                    GetGradientPos = (LEDCount - ( (Progress[i] + LedID) - LEDCount));
                }
                else
                {
                    GetGradientPos = (Progress[i] + LedID);
                }

                if (GetGradientPos <= 0)
                {
                    GetGradientPos *= -1;
                }

                int RGBCol[3];

                for (int CVal = 0; CVal < 3; CVal++)
                {
                    RGBCol[CVal] = int(S[CVal] + (float(GetGradientPos)/float(LEDCount))*(F[CVal]-S[CVal]));
                }

                controller_zone->SetLED((RVRS ?  ((LEDCount - 1) - LedID) :  LedID), ToRGBColor(RGBCol[0],RGBCol[1],RGBCol[2]), Brightness, Temperature, Tint);
            }

            float speed_mult = LEDCount > 0 ? 0.1 * (float) LEDCount : 1.f;

            if (Progress[i] < (LEDCount*2))
            {
                Progress[i] += speed_mult * ((float)Speed / (float)FPS);
            }
            else if (Progress[i] >= (LEDCount*2))
            {
                Progress[i] = 0;
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int ColumnCount = controller_zone->matrix_map_width();
            int RowCount = controller_zone->matrix_map_height();

            for (int ColumnID = 0; ColumnID < ColumnCount; ColumnID++)
            {
                float GetGradientPos;

                if ((Progress[i] + ColumnID) > ColumnCount)
                {
                    GetGradientPos = (ColumnCount - ( (Progress[i] + ColumnID) - ColumnCount));
                }
                else
                {
                    GetGradientPos = (Progress[i] + ColumnID);
                }

                if (GetGradientPos <= 0)
                {
                    GetGradientPos *= -1;
                }

                int RGBCol[3];

                for (int CVal = 0; CVal < 3; CVal++)
                {
                    RGBCol[CVal] = int(S[CVal] + (float(GetGradientPos)/float(ColumnCount))*(F[CVal]-S[CVal]));
                }

                for (int RowID = 0; RowID < RowCount; RowID++)
                {
                    int LedID = controller_zone->map()[((RowID * ColumnCount) + (RVRS ? (ColumnCount - 1) - ColumnID: ColumnID ) )];
                    controller_zone->SetLED(LedID,ToRGBColor(RGBCol[0],RGBCol[1],RGBCol[2]), Brightness, Temperature, Tint);
                }
            }

            if (Progress[i] < (ColumnCount*2))
            {
                Progress[i] += ((float)Speed / (float)FPS);
            }
            else if (Progress[i] >= (ColumnCount*2))
            {
                Progress[i] = 0;
            }
        }

        i++;
    }
}


void GradientWave::OnControllerZonesListChanged(std::vector<ControllerZone*>)
{
    Progress.clear();
}

void GradientWave::SetRandomColorsEnabled(bool value)
{
    RandomColorsEnabled = value;

    if (RandomColorsEnabled)
    {
        RandomColorList[0] = ColorUtils::RandomRGBColor();
        RandomColorList[1] = ColorUtils::RandomRGBColor();
    }
}
