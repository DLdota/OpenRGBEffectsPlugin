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
    EffectDetails.EffectDescription = "Progressivly fills your devices with a defined color";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 10;
    EffectDetails.UserColors   = 1;

    random = ColorUtils::RandomRGBColor();

    SetSpeed(50);
}

Fill::~Fill()
{
    delete ui;
}

void Fill::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(ControllerZone* controller_zone: controller_zones)
    {
        zone_type ZT = controller_zone->type();
        bool reverse = controller_zone->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            int leds_count = controller_zone->leds_count();

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID, leds_count);
                controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                RGBColor color = GetColor(reverse ? cols - col_id - 1: col_id, cols);

                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                    controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
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
    RGBColor color = RandomColorsEnabled ? random : UserColors[0];
    if(((long) floorf(time)) % 2 == 1)
    {
        return ColorUtils::Enlight(color, 1 - progress);
    }

    double position = progress * width;

    double distance = position - idx;

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
