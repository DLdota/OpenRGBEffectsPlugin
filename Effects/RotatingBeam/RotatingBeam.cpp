#include "RotatingBeam.h"
#include "ColorUtils.h"

REGISTER_EFFECT(RotatingBeam);

RotatingBeam::RotatingBeam(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::RotatingBeam)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "RotatingBeam";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 50;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Glow";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    ui->mode->addItems(modes);
}

RotatingBeam::~RotatingBeam()
{
    delete ui;
}

void RotatingBeam::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

QLineF RotatingBeam::Rotate()
{
    QPointF p1, p2;

    switch (mode)
    {
    case CLOCKWISE:
    {
        float x = cos(progress);
        float y = sin(progress);

        p1.setX(x);
        p1.setY(y);
        p2.setX(1 - x);
        p2.setY(1 - y);

        break;

    }
    case COUNTER_CLOCKWISE:
    {
        float x = cos(-progress);
        float y = sin(-progress);

        p1.setX(x);
        p1.setY(y);
        p2.setX(1 - x);
        p2.setY(1 - y);

        break;

    }
    case PENDULUM:
    {
        float x = 0.5 * (1 + cos(progress));

        p1.setX(0.5);
        p1.setY(0);
        p2.setX(x);
        p2.setY(1);

        break;

    }
    case WIPERS:
    {
        float x = 0.5 * (1 + cos(progress));

        p1.setX(x);
        p1.setY(0);
        p2.setX(0.5);
        p2.setY(1);

        break;

    }
    case SWING_H:
    {
        float x = 0.5 * (1 + cos(progress));

        p1.setX(0);
        p1.setY(x);
        p2.setX(1);
        p2.setY(1 - x);

        break;

    }
    case SWING_V:
    {
        float x = 0.5 * (1 + cos(progress));

        p1.setX(x);
        p1.setY(0);
        p2.setX(1 - x);
        p2.setY(1);

        break;

    }
    default: return QLineF({0, 0}, {1, 1});
    }

    return QLineF(p1, p2);
}

void RotatingBeam::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    QLineF line = Rotate();

    for(ControllerZone* controller_zone : controller_zones)
    {
        unsigned int start_idx = controller_zone->start_idx();
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = width;

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(i, width * 0.25, line, width - 1, height - 1);
                controller_zone->controller->SetLED(start_idx + i, color);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();
            unsigned int * map = controller_zone->map();

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    RGBColor color = GetColor(w, h, line, width - 1, height - 1);

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

RGBColor RotatingBeam::GetColor(float x0, float y0, QLineF l, float w, float h)
{
    float glow = 0.01 * Slider2Val;

    float x1 = l.p1().x() * w;
    float x2 = l.p2().x() * w;

    float y1 = l.p1().y() * h;
    float y2 = l.p2().y() * h;

    float distance = fabs((x2 - x1) * (y1 - y0) - (x1 - x0) * (y2 - y1)) / sqrt(pow(x2 - x1 ,2) + pow(y2 - y1,2));

    hsv_t hsv_tmp;

    hsv_tmp.value = hsv1.value - hsv1.value * pow(distance / (0.5*(w+h)) , glow);
    hsv_tmp.hue = hsv1.hue;
    hsv_tmp.saturation = hsv1.saturation;

    RGBColor color1 = RGBColor(hsv2rgb(&hsv_tmp));

    RGBColor color2 = RGBColor(hsv2rgb(&hsv2));

    return ColorUtils::Interpolate(color2, color1, 1 - distance / ((w+h)) );
}


void RotatingBeam::SetUserColors(std::vector<RGBColor> colors)
{
    UserColors = colors;

    ResetUserColors();
}

void RotatingBeam::SetRandomColorsEnabled(bool value)
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


void RotatingBeam::ResetUserColors()
{
    rgb2hsv(UserColors[0], &hsv1);
    rgb2hsv(UserColors[1], &hsv2);
}

void RotatingBeam::on_mode_currentIndexChanged(int value)
{
    mode = static_cast<RotatingBeamMode>(value);
}

void RotatingBeam::LoadCustomSettings(json settings)
{
    if(settings.contains("mode"))
        ui->mode->setCurrentIndex(settings["mode"]);
}

json RotatingBeam::SaveCustomSettings(json settings)
{
    settings["mode"] = mode;
    return settings;
}
