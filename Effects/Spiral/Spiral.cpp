#include "Spiral.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Spiral);

Spiral::Spiral(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Spiral)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Spiral";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Spiral effect";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 500;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 1;
    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Spiral shape";
    EffectDetails.HasCustomSettings = true;

    SetSpeed(200);
    SetSlider2Val(10);
}

Spiral::~Spiral()
{
    delete ui;
}

void Spiral::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Spiral::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i]->start_idx();
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();
        bool reverse = controller_zones[i]->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0, leds_count * 0.5, 0.5, reverse);
                controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i]->matrix_map_width();
            int rows = controller_zones[i]->matrix_map_height();

            float cx = (cols - 1) * 0.5;
            float cy = (rows - 1) * 0.5;

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

    time += Speed / (float) FPS;
}

RGBColor Spiral::GetColor(float x, float y, float cx, float cy, bool reverse)
{
    double angle    = (reverse ? atan2(x - cx, y - cy) :  - atan2(x - cx, y - cy)) * 180 / 3.14159265359;
    double distance = sqrt(pow(cx - x, 2) + pow(cy - y, 2));

    hsv_t hsv;

    if(RandomColorsEnabled)
    {
        hsv.value = 255;
        hsv.hue = abs((int)(angle + Slider2Val * distance - time) % 360);
        hsv.saturation = 255;
    }
    else
    {
        rgb2hsv(UserColors[0], &hsv);
        hsv.value = 255 - (abs((int)(angle + Slider2Val * distance - time) % 360) / 360.f) * 255;
    }

    return RGBColor(hsv2rgb(&hsv));
}


