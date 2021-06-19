#include "ColorWheelEffect.h"
#include "hsv.h"

REGISTER_EFFECT(ColorWheelEffect);

ColorWheelEffect::ColorWheelEffect(QWidget *parent) :
    QWidget(parent),
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
}

ColorWheelEffect::~ColorWheelEffect()
{
    delete ui;
}


void ColorWheelEffect::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}


void ColorWheelEffect::StepEffect(std::vector<ControllerZone> controller_zones)
{

    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i].start_idx();
        zone_type ZT = controller_zones[i].type();
        int leds_count = controller_zones[i].leds_count();
        bool reverse = controller_zones[i].reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            double cx = leds_count / 2.f;
            double cy = 0;

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0, cx, cy, reverse);
                controller_zones[i].controller->SetLED(start_idx + LedID, color);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i].matrix_map_width();
            int rows = controller_zones[i].matrix_map_height();

            double cx = cols / 2.f;
            double cy = rows / 2.f;

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(col_id, row_id, cx, cy, reverse);
                    int LedID = controller_zones[i].controller->zones[controller_zones[i].zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i].controller->SetLED(start_idx + LedID, color);
                }
            }
        }
    }

    progress += (float) Speed / (float) FPS;
}


RGBColor ColorWheelEffect::GetColor(unsigned int x, unsigned int y, double cx, double cy, bool reverse){
    float hue = (float)(progress + (int)(180 + (reverse?atan2(y - cy, x - cx):atan2(x - cx, y - cy)) * (180.0 / 3.14159)) % 360);
    hsv_t hsv = { 0, 0, 0 };
    hsv.hue = (int)hue;
    hsv.saturation = 255;
    hsv.value = 255;

    return RGBColor(hsv2rgb(&hsv));
}

