#include "Swap.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Swap);

Swap::Swap(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Swap)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Swap";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Alternate two colors on your devices from left to right";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 20;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;

    random1 = ColorUtils::RandomRGBColor();
    random2 = ColorUtils::RandomRGBColor();

    SetSpeed(10);
}

Swap::~Swap()
{
    delete ui;
}

void Swap::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();
        bool reverse = controller_zones[i]->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID, leds_count);
                controller_zones[i]->SetLED(LedID, color, Brightness);
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
                    controller_zones[i]->SetLED(LedID, color, Brightness);
                }
            }

        }
    }

    time += 0.1 * Speed / (float) FPS;

    float whole;

    progress = std::modf(time, &whole);
    dir = (int) whole % 2;

    c1 = RandomColorsEnabled ? random1 : UserColors[0];
    c2 = RandomColorsEnabled ? random2 : UserColors[1];

    if(old_dir == 0 && dir == 1)
    {
        random1 = ColorUtils::RandomRGBColor();
    }
    else if(old_dir == 1 && dir == 0)
    {
        random2 = ColorUtils::RandomRGBColor();
    }

    old_dir = dir;
}

RGBColor Swap::GetColor(unsigned int i, unsigned int w)
{
    float x = dir ? progress : 1 - progress;
    return (i+1) <= x * (w+1) ? c1 : c2;
}
