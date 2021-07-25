#include "Fill.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Fill);

Fill::Fill(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Fill)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Fill";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Fills the zones";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 10;
    EffectDetails.UserColors   = 1;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = false;
    EffectDetails.HasCustomSettings = false;

    random = ColorUtils::RandomRGBColor();
}

Fill::~Fill()
{
    delete ui;
}

void Fill::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Fill::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i]->start_idx();
        zone_type ZT = controller_zones[i]->type();
        bool reverse = controller_zones[i]->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            int leds_count = controller_zones[i]->leds_count();

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID, leds_count);
                controller_zones[i]->controller->SetLED(start_idx + LedID, color);
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
                    controller_zones[i]->controller->SetLED(start_idx + LedID, color);
                }
            }
        }
    }

    time += 0.01 * Speed / (float) FPS;
    float whole;

    old_progress = progress;

    progress = std::modf(time, &whole);

    if(old_progress > progress && RandomColorsEnabled)
    {
        random = ColorUtils::RandomRGBColor();
    }
}

RGBColor Fill::GetColor(unsigned int idx, unsigned int width)
{
    double position = progress * width;

    double distance = position - idx;

    RGBColor color = RandomColorsEnabled ? random : UserColors[0];

    if(distance > 1)
    {
        return color;
    }

    if(distance > 0)
    {
        return ColorUtils::Enlight(color, distance);
    }

    return ColorUtils::OFF();
}
