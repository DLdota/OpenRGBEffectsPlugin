#include "CustomMarquee.h"

REGISTER_EFFECT(CustomMarquee);

CustomMarquee::CustomMarquee(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::CustomMarquee)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "CustomMarquee";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Create your own marquee effect";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 50;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    SetSpeed(25);

}

CustomMarquee::~CustomMarquee()
{
    delete ui;
}

void CustomMarquee::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(ControllerZone* controller_zone: controller_zones)
    {
        zone_type ZT = controller_zone->type();
        int leds_count = controller_zone->leds_count();
        bool reverse = controller_zone->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID);
                controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                RGBColor color = GetColor(reverse ? cols - col_id - 1: col_id);

                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                    controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
                }
            }
        }
    }

    progress += Speed / (float) FPS;
}

RGBColor CustomMarquee::GetColor(unsigned int i)
{
    std::vector<RGBColor> colors = ui->colorsPicker->Colors();
    int index = (i + (int) progress) % colors.size();
    return colors[index];
}

void CustomMarquee::LoadCustomSettings(json settings)
{
    if (settings.contains("colors"))
    {
        ui->colorsPicker->SetColors(settings["colors"]);
    }
}

json CustomMarquee::SaveCustomSettings()
{
    json settings;

    settings["colors"] = ui->colorsPicker->Colors();

    return settings;
}
