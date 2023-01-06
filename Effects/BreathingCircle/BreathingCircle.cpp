#include "BreathingCircle.h"
#include "ColorUtils.h"

REGISTER_EFFECT(BreathingCircle);

BreathingCircle::BreathingCircle(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::BreathingCircle)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "BreathingCircle";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A breathing circle effect";
    EffectDetails.IsReversable = true;
    EffectDetails.SupportsRandom = false;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 10;
    EffectDetails.UserColors   = 1;
    EffectDetails.MaxSlider2Val = 20;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Thickness";
}

BreathingCircle::~BreathingCircle()
{
    delete ui;
}

void BreathingCircle::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i]->start_idx();
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0, leds_count, 1);
                controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
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
                    RGBColor color = GetColor(col_id, row_id, cols-1, rows-1);

                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
                }
            }

        }
    }

    time += (float) Speed / (float) FPS;
    progress = 0.35 * (1 + sin(0.1 * time));

}

RGBColor BreathingCircle::GetColor(float x, float y, float w, float h)
{
    float distance = std::min<float>(1.0, sqrt(pow(0.5 - x/w, 2) + pow(0.5 - y/h, 2)));

    if(distance > progress || distance < progress - Slider2Val/(0.5*(w+h)))
        return ColorUtils::OFF();

    return UserColors[0];
}
