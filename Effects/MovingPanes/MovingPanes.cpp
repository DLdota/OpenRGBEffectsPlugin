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
    EffectDetails.EffectDescription = "Moving panes effect";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 50;
    EffectDetails.MinSlider2Val = 2;
    EffectDetails.Slider2Name   = "Divisions";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;
}

MovingPanes::~MovingPanes()
{
    delete ui;
}

void MovingPanes::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void MovingPanes::StepEffect(std::vector<ControllerZone*> controller_zones)
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
                RGBColor color = GetColor(LedID, LedID, leds_count, leds_count, reverse);
                controller_zones[i]->controller->SetLED(start_idx + LedID, color);
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
                    RGBColor color = GetColor(col_id, row_id, cols, rows, reverse);

                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->controller->SetLED(start_idx + LedID, color);
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