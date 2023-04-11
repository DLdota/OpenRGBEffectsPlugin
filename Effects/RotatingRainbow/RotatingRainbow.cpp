#include "RotatingRainbow.h"

REGISTER_EFFECT(RotatingRainbow);

RotatingRainbow::RotatingRainbow(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::RotatingRainbow)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "RotatingRainbow";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A rainbow that rotates arround the center of your devices";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Color speed";

    SetSpeed(20);
    SetSlider2Val(30);
}

RotatingRainbow::~RotatingRainbow()
{
    delete ui;
}

void RotatingRainbow::StepEffect(std::vector<ControllerZone*> controller_zones)
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
                RGBColor color = GetColor(LedID, 0.5, leds_count  * 0.5, 0.5, reverse);
                controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            float cx = (cols - 1) * 0.5;
            float cy = (rows - 1) * 0.5;

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                   RGBColor color = GetColor(col_id, row_id, cx, cy, reverse);
                   int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                   controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
                }
            }

        }
    }

    time += 0.1 * Speed / (float) FPS;

    cos_time = cos(time);
    sin_time = sin(time);

    cos_reverse_time = cos(-time);
    sin_reverse_time = sin(-time);
}


RGBColor RotatingRainbow::GetColor(float x, float y, float cx, float cy, bool reverse)
{
    double c = reverse? cos_reverse_time : cos_time;
    double s = reverse? sin_reverse_time : sin_time;

    hsv_t hsv;

    hsv.hue = (int)(time * Slider2Val + 360 * ((y - cy) * 2 * c + (x - cx) * 2 * s) / 128) % 360;
    hsv.value = 255;
    hsv.saturation = 255;

    return RGBColor(hsv2rgb(&hsv));
}

