#include "Marquee.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Marquee);

Marquee::Marquee(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Marquee)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Marquee";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A simple marquee for your devices";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 1;
    EffectDetails.MaxSlider2Val = 20;
    EffectDetails.MinSlider2Val = 2;
    EffectDetails.Slider2Name   = "Spacing";

    random.saturation = 255;
    random.value = 255;
    random.hue = 0;

    SetSpeed(50);
}

Marquee::~Marquee()
{
    delete ui;
}

void Marquee::StepEffect(std::vector<ControllerZone*> controller_zones)
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
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID);
                controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                RGBColor color = GetColor(reverse ? cols - col_id - 1: col_id);

                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                    controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
                }
            }

        }
    }

    progress += 0.1 * Speed / (float) FPS;

    if(RandomColorsEnabled)
    {
        random.hue++;
        random.hue%=360;
    }
}

RGBColor Marquee::GetColor(unsigned int i)
{
    unsigned int spacing = Slider2Val;

    return (i + (int) progress) % spacing == 0 ?
                RandomColorsEnabled ?
                    RGBColor(hsv2rgb(&random)) : UserColors[0]
              : ColorUtils::OFF();
}
