#include "SwirlCircles.h"
#include "ColorUtils.h"

REGISTER_EFFECT(SwirlCircles);

SwirlCircles::SwirlCircles(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::SwirlCircles)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "SwirlCircles";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Rotating circles";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Glow";

    EffectDetails.HasCustomWidgets = false;
    EffectDetails.HasCustomSettings = false;

}

SwirlCircles::~SwirlCircles()
{
    delete ui;
}


void SwirlCircles::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}


void SwirlCircles::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(ControllerZone* controller_zone : controller_zones)
    {
        unsigned int start_idx = controller_zone->start_idx();
        bool reverse = controller_zone->reverse;
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            float hx = 0.5 * width;
            float hy = 0.5 * height;

            float y1 = hy + hy * sin(reverse ? -progress : progress);
            float x1 = hx + hx * cos(reverse ? -progress : progress);

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(i, 0, width, height, x1, y1);
                controller_zone->controller->SetLED(start_idx + i, color);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();
            unsigned int * map = controller_zone->map();

            float hx = 0.5 * width;
            float hy = 0.5 * height;

            float y1 = hy + hy * sin(reverse ? -progress : progress);
            float x1 = hx + hx * cos(reverse ? -progress : progress);

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    RGBColor color = GetColor(w, h, width, height, x1, y1);

                    unsigned int led_num = map[h * width + w];
                    controller_zone->controller->SetLED(start_idx + led_num,color);
                }
            }

        }
    }

    progress +=  0.1 * (float) Speed / (float) FPS;

    if(RandomColorsEnabled)
    {
        hsv1.hue++;
        hsv2.hue++;
    }
}

RGBColor SwirlCircles::GetColor(unsigned int x, unsigned int y, unsigned int w, unsigned int h, float x1, float y1)
{
    float glow = 0.01 * Slider2Val;

    double distance1 = sqrt(pow(x1 - x, 2) + pow(y1 - y, 2));
    float distance1_percent = distance1 < radius ? 0 : pow(distance1 / (h+w), glow);

    hsv_t hsv_tmp;

    hsv_tmp.value = hsv1.value * (1 - distance1_percent);
    hsv_tmp.hue = hsv1.hue;
    hsv_tmp.saturation = hsv1.saturation;

    RGBColor color1 = RGBColor(hsv2rgb(&hsv_tmp));

    float y2 = h - y1;
    float x2 = w - x1;

    double distance2 = sqrt(pow(x2 - x, 2) + pow(y2 - y, 2));
    float distance2_percent = distance2 < radius ? 0 : pow(distance2 / (h+w), glow);

    hsv_tmp.value = hsv2.value * (1 - distance2_percent);
    hsv_tmp.hue = hsv2.hue;
    hsv_tmp.saturation = hsv2.saturation;

    RGBColor color2 = RGBColor(hsv2rgb(&hsv_tmp));

    return ColorUtils::Screen(color1, color2);
}

void SwirlCircles::SetUserColors(std::vector<RGBColor> colors)
{
    UserColors = colors;

    ResetUserColors();
}

void SwirlCircles::SetRandomColorsEnabled(bool value)
{
    RandomColorsEnabled = value;

    if(!RandomColorsEnabled)
    {
        ResetUserColors();
    }
    else
    {
        hsv1.hue = 0;
        hsv1.value = 255;
        hsv1.saturation = 255;

        hsv2.hue = 180;
        hsv2.value = 255;
        hsv2.saturation = 255;
    }
}


void SwirlCircles::ResetUserColors()
{
    rgb2hsv(UserColors[0], &hsv1);
    rgb2hsv(UserColors[1], &hsv2);
}

void SwirlCircles::on_radius_valueChanged(int value)
{
    radius = value;
}


void SwirlCircles::LoadCustomSettings(json Settings)
{
    if (Settings.contains("radius")) radius    = Settings["radius"];

    ui->radius->setValue(radius);
}

json SwirlCircles::SaveCustomSettings(json Settings)
{
    Settings["radius"]    = radius;
    return Settings;
}
