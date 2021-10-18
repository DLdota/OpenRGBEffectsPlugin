#include "Comet.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Comet);

Comet::Comet(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Comet)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Comet";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Comet effect";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 50;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 1;
    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Comet size";
    EffectDetails.HasCustomSettings = true;

    SetSpeed(25);
    SetSlider2Val(50);
}

Comet::~Comet()
{
    delete ui;
}

void Comet::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Comet::StepEffect(std::vector<ControllerZone*> controller_zones)
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
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID, leds_count);
                controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i]->matrix_map_width();
            int rows = controller_zones[i]->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                RGBColor color = GetColor(reverse ? cols - col_id - 1: col_id, cols);

                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
                }
            }

        }
    }

    time += 0.01 * Speed / (float) FPS;
    float whole;
    progress = std::modf(time, &whole);
}

RGBColor Comet::GetColor(unsigned int i, unsigned int width)
{
    float comet_size = 0.01 * Slider2Val * width;

    float position = progress * 2 * width;

    if(i > position)
    {
        return ColorUtils::OFF();
    }

    float distance = position - i;

    double value;

    if(distance > comet_size)
    {
        value = 0;
    }
    else if(distance == 0)
    {
        value = 1;
    }
    else
    {
        value = 1 - (distance / comet_size);
    }

    if(RandomColorsEnabled)
    {
        tmp.hue = (int) (1000 * (time) + (distance / comet_size) * 360) % 360;
        tmp.saturation = pow(value, 0.2) * 255;
        tmp.value = pow(value, 3) * 255;
    }
    else
    {
        hsv_t user_hsv;
        rgb2hsv(UserColors[0], &user_hsv);

        tmp.hue = user_hsv.hue;
        tmp.saturation = pow(value, 0.2) * user_hsv.saturation;
        tmp.value = pow(value, 3) * user_hsv.value;
    }

    return RGBColor(hsv2rgb(&tmp));
}
