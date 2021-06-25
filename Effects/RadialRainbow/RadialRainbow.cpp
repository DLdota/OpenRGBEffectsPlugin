#include "RadialRainbow.h"
#include "hsv.h"

REGISTER_EFFECT(RadialRainbow);

RadialRainbow::RadialRainbow(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::RadialRainbow)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Radial Rainbow";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Radial rainbow wave";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 10;
    EffectDetails.UserColors   = 0;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Frequency";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;
}

RadialRainbow::~RadialRainbow()
{
    delete ui;
}

void RadialRainbow::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}


void RadialRainbow::StepEffect(std::vector<ControllerZone> controller_zones)
{
    float cx_shift_mult = cx_shift / 100.f;
    float cy_shift_mult = cy_shift / 100.f;

    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i].start_idx();
        zone_type ZT = controller_zones[i].type();
        int leds_count = controller_zones[i].leds_count();
        bool reverse = controller_zones[i].reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            double cx = leds_count * cx_shift_mult;
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

            double cx = cols * cx_shift_mult;
            double cy = rows * cy_shift_mult;

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


RGBColor RadialRainbow::GetColor(unsigned int x, unsigned int y, double cx, double cy, bool reverse)
{
    double distance = sqrt(pow(cx - x, 2) + pow(cy - y, 2));

    float width = Slider2Val * 0.5f;

    hsv_t hsv;
    hsv.hue = distance * width + (reverse ? progress:-progress);
    hsv.hue %= 360;
    hsv.saturation = 255;
    hsv.value = 255;

    return RGBColor(hsv2rgb(&hsv));
}

void RadialRainbow::on_cx_valueChanged(int value)
{
    cx_shift = value;
}

void RadialRainbow::on_cy_valueChanged(int value)
{
    cy_shift = value;
}

void RadialRainbow::LoadCustomSettings(json settings)
{
    if(settings.contains("cx")) cx_shift = settings["cx"];
    if(settings.contains("cy")) cy_shift = settings["cy"];

    ui->cx->setValue(cx_shift);
    ui->cy->setValue(cy_shift);
}

json RadialRainbow::SaveCustomSettings(json settings)
{
    settings["cx"] = cx_shift;
    settings["cy"] = cy_shift;
    return settings;
}
