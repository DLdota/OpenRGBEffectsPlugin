#include "MovingPanes.h"
#include "ColorUtils.h"

REGISTER_EFFECT(MovingPanes);

MovingPanes::MovingPanes(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::MovingPanes)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "MovingPanes";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Parts of your devices in symmetrical motion";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.MaxSlider2Val = 50;
    EffectDetails.MinSlider2Val = 2;
    EffectDetails.Slider2Name   = "Divisions";
    EffectDetails.SupportsRandom = false;

    SetSpeed(50);
    SetSlider2Val(4);
}

MovingPanes::~MovingPanes()
{
    delete ui;
}

void MovingPanes::StepEffect(std::vector<ControllerZone*> controller_zones)
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
                RGBColor color = GetColor(LedID, LedID, leds_count, leds_count, reverse);
                controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(col_id, row_id, cols, rows, reverse);
                    int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                    controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
                }
            }

        }
    }

    time += 0.1 * (float) Speed / (float) FPS;
}

RGBColor MovingPanes::GetColor(float x, float y, float w, float h, bool reverse)
{
    int zone = x / (w / Slider2Val);
    int zone_id = zone % 2;
    float pi4 = 3.14 * 0.25;
    float t = reverse ? time : -time;

    float s = 0.5 * (1 + sin((y) / (h * 0.25) + (zone_id ? 1 : -1) * t + pi4));

    return ColorUtils::Interpolate(UserColors[zone_id ? 1 : 0], UserColors[zone_id ? 0 : 1], s);
}
