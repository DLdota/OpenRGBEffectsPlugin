#include "Ambient.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Ambient);

Ambient::Ambient() : RGBEffect()
{
    EffectDetails.EffectName = "Ambient";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Takes a portion of the screen and\nsets all of the LEDs to it";
    EffectDetails.IsReversable = false;

    EffectDetails.MaxSpeed = 0;
    EffectDetails.MinSpeed = 0;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.UserColors = 0;

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    SCRNSLCT = new ScreenSelection();
}

Ambient::~Ambient()
{
    delete SCRNSLCT;
}

void Ambient::DefineExtraOptions(QLayout* ParentLayout)
{    
    ParentLayout->addWidget(SCRNSLCT);
}

void Ambient::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    SCRNSLCT->GetScreen();

    if(SCRNSLCT->GetCalcType() == SCREEN_COPY_MODE)
    {
        for(ControllerZone* controller_zone : controller_zones)
        {
            unsigned int start_idx = controller_zone->start_idx();
            zone_type ZT = controller_zone->type();

            if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
            {
                unsigned int width = controller_zone->leds_count();
                unsigned int height = 1;


                QImage scaled = SCRNSLCT->GetImage(width, height);

                for(unsigned int i = 0; i < width; i++)
                {
                    QColor color = scaled.pixelColor(i, 0);
                    controller_zone->controller->SetLED(start_idx + i, ColorUtils::fromQColor(color));
                }

            }
            else if(ZT == ZONE_TYPE_MATRIX)
            {
                unsigned int width = controller_zone->matrix_map_width();
                unsigned int height = controller_zone->matrix_map_height();
                unsigned int * map = controller_zone->map();

                QImage scaled = SCRNSLCT->GetImage(width, height);

                for(unsigned int h = 0; h < height; h++)
                {
                    for(unsigned int w = 0; w <  width; w++)
                    {
                        QColor color = scaled.pixelColor(w, h);

                        unsigned int led_num = map[h * width + w];
                        controller_zone->controller->SetLED(start_idx + led_num, ColorUtils::fromQColor(color));
                    }
                }

            }
        }
    }
    else
    {
        QColor C = SCRNSLCT->CalcColor();

        RGBColor color = ColorUtils::fromQColor(C);

        for (ControllerZone* controller_zone : controller_zones)
        {
            controller_zone->controller->SetAllZoneLEDs(controller_zone->zone_idx, color);
        }
    }


}

void Ambient::EffectState(bool Auto)
{
    SCRNSLCT->SetAuto(!Auto);
}

void Ambient::LoadCustomSettings(json Settings)
{
    SCRNSLCT->SetCalcType(Settings["CalcMode"]);
    SCRNSLCT->SetShowState(Settings["PreviewShowing"]);
}

json Ambient::SaveCustomSettings(json Settings)
{
    Settings["CalcMode"] = SCRNSLCT->GetCalcType();
    Settings["PreviewShowing"] = !SCRNSLCT->GetShowState();

    return Settings;
}
