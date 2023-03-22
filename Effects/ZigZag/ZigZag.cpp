#include "ZigZag.h"
#include "ColorUtils.h"

REGISTER_EFFECT(ZigZag);

ZigZag::ZigZag(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::ZigZag)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "ZigZag";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A snake moving on your matrix typed devices";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 20;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 1;

    SetSpeed(10);
}

ZigZag::~ZigZag()
{
    delete ui;
}

void ZigZag::StepEffect(std::vector<ControllerZone*> controller_zones)
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
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID, 0, leds_count, 1);
                controller_zones[i]->SetLED(LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i]->matrix_map_width();
            int rows = controller_zones[i]->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(reverse ? cols - col_id - 1: col_id, reverse ? rows - row_id - 1: row_id, cols, rows);
                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->SetLED(LedID, color, Brightness);
                }
            }
        }
    }

    time += 0.01 * (float) Speed / (float) FPS;

    progress = 2*(time-(long)time);
}



RGBColor ZigZag::GetColor(float x, float y, float w, float h)
{       
    float total_leds = w * h;
    float current_led_position = ((int) x % 2 == 0 ? y : h - y - 1 ) + x * h;
    float current_led_percent = current_led_position/total_leds;

    if(current_led_percent < progress)
    {
        float distance = pow(current_led_percent / progress, 3);

        hsv_t hsv;

        if(RandomColorsEnabled)
        {
            hsv.hue = int(distance*360. -100.*time) % 360;
            hsv.saturation = 255;
            hsv.value = 255;
        }
        else
        {
            rgb2hsv(UserColors[0], &hsv);
        }

        hsv.value *= distance;
        return RGBColor(hsv2rgb(&hsv));
    }
    else
    {
        return ColorUtils::OFF();
    }

}

