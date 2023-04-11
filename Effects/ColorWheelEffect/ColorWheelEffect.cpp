#include "ColorWheelEffect.h"
#include "hsv.h"

REGISTER_EFFECT(ColorWheelEffect);

ColorWheelEffect::ColorWheelEffect(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::ColorWheelEffect)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Color Wheel";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A rotating rainbow";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 10;
    EffectDetails.HasCustomSettings = true;

    SetSpeed(100);

    ui->direction->addItems({"Clockwise", "Counter-clockwise"});
}

ColorWheelEffect::~ColorWheelEffect()
{
    delete ui;
}

void ColorWheelEffect::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    float cx_shift_mult = cx_shift / 100.f;
    float cy_shift_mult = cy_shift / 100.f;

    for(ControllerZone* controller_zone: controller_zones)
    {
        zone_type ZT = controller_zone->type();
        int leds_count = controller_zone->leds_count();
        bool reverse = controller_zone->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            double cx = leds_count * cx_shift_mult;
            double cy = 0;

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0, cx, cy, reverse);
                controller_zone->SetLED(LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            double cx = (cols - 1) * cx_shift_mult;
            double cy = (rows - 1) * cy_shift_mult;

            for (int col_id = 0; col_id < cols; col_id++)
            {
                 for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(col_id, row_id, cx, cy, reverse);
                    int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                    controller_zone->SetLED(LedID, color, Brightness);
                }
            }
        }
    }

    progress += (float) Speed / (float) FPS;
}


RGBColor ColorWheelEffect::GetColor(unsigned int x, unsigned int y, double cx, double cy, bool reverse)
{
    float direction_mult = direction == 0 ? 1.f : -1.f;
    float hue = (float)(progress + (int)(180 + direction_mult * (reverse ? atan2(y - cy, x - cx) : atan2(x - cx, y - cy)) * (180.0 / 3.14159)) % 360);
    hsv_t hsv = { 0, 0, 0 };
    hsv.hue = (int)hue;
    hsv.saturation = 255;
    hsv.value = 255;

    return RGBColor(hsv2rgb(&hsv));
}

void ColorWheelEffect::on_cx_valueChanged(int value)
{
    cx_shift = value;
}

void ColorWheelEffect::on_cy_valueChanged(int value)
{
    cy_shift = value;
}

void ColorWheelEffect::on_direction_currentIndexChanged(int value)
{
    direction = value;
}

void ColorWheelEffect::LoadCustomSettings(json settings)
{
    if(settings.contains("cx")) ui->cx->setValue(settings["cx"]);
    if(settings.contains("cy")) ui->cy->setValue(settings["cy"]);
    if(settings.contains("direction")) ui->direction->setCurrentIndex(settings["direction"]);
}

json ColorWheelEffect::SaveCustomSettings()
{
    json settings;

    settings["cx"] = cx_shift;
    settings["cy"] = cy_shift;
    settings["direction"] = direction;

    return settings;
}


