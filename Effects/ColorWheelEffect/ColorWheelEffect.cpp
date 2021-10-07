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
    EffectDetails.UserColors   = 0;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    ui->direction->addItems({"Clockwise", "Counter-clockwise"});
}

ColorWheelEffect::~ColorWheelEffect()
{
    delete ui;
}


void ColorWheelEffect::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}


void ColorWheelEffect::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    float cx_shift_mult = cx_shift / 100.f;
    float cy_shift_mult = cy_shift / 100.f;

    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i]->start_idx();
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();
        bool reverse = controller_zones[i]->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            double cx = leds_count * cx_shift_mult;
            double cy = 0;

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0, cx, cy, reverse);
                controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i]->matrix_map_width();
            int rows = controller_zones[i]->matrix_map_height();

            double cx = (cols - 1) * cx_shift_mult;
            double cy = (rows - 1) * cy_shift_mult;

            for (int col_id = 0; col_id < cols; col_id++)
            {
                 for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(col_id, row_id, cx, cy, reverse);
                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
                }
            }
        }
    }

    progress += (float) Speed / (float) FPS;
}


RGBColor ColorWheelEffect::GetColor(unsigned int x, unsigned int y, double cx, double cy, bool reverse){
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
    if(settings.contains("cx")) cx_shift = settings["cx"];
    if(settings.contains("cy")) cy_shift = settings["cy"];
    if(settings.contains("direction")) direction = settings["direction"];

    ui->cx->setValue(cx_shift);
    ui->cy->setValue(cy_shift);
    ui->direction->setCurrentIndex(direction);
}

json ColorWheelEffect::SaveCustomSettings(json settings)
{
    settings["cx"] = cx_shift;
    settings["cy"] = cy_shift;
    settings["direction"] = direction;
    return settings;
}


