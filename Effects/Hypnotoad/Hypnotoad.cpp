#include "Hypnotoad.h"
#include "hsv.h"
#include "ColorUtils.h"

#include <QImage>
#include <QPainter>

REGISTER_EFFECT(Hypnotoad);

Hypnotoad::Hypnotoad(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Hypnotoad)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Hypnotoad";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "You wont escape this";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 0;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    ui->color_rotation_direction->addItems({"Clockwise","Counter-clockwise"});
    ui->animation_direction->addItems({"To the inside","To the outside"});
}

Hypnotoad::~Hypnotoad()
{
    delete ui;
}

void Hypnotoad::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}


void Hypnotoad::StepEffect(std::vector<ControllerZone> controller_zones)
{
    float cx_shift_mult = cx_shift / 100.f;
    float cy_shift_mult = cy_shift / 100.f;

    for(ControllerZone& controller_zone : controller_zones)
    {
        unsigned int start_idx = controller_zone.start_idx();
        bool reverse = controller_zone.reverse;

        if(controller_zone.type() == ZONE_TYPE_SINGLE || controller_zone.type() == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone.leds_count();
            unsigned int height = 1;

            float cx = width * cx_shift_mult;
            float cy = height * cy_shift_mult;

            for(unsigned int i = 0; i < width; i++)
            {
                QColor color = GetColor(i, 0, cx, cy, reverse);
                controller_zone.controller->SetLED(start_idx + i, ColorUtils::fromQColor(color));
            }

        }
        else if(controller_zone.type() == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone.matrix_map_width();
            unsigned int height = controller_zone.matrix_map_height();
            unsigned int * map = controller_zone.map();

            float cx = width * cx_shift_mult;
            float cy = height * cy_shift_mult;

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    QColor color = GetColor(w, h, cx, cy, reverse);

                    unsigned int led_num = map[h * width + w];
                    controller_zone.controller->SetLED(start_idx + led_num, ColorUtils::fromQColor(color));
                }
            }

        }
    }

    progress +=  0.1 * (float) Speed / (float) FPS;
}


RGBColor Hypnotoad::GetColor(unsigned int x, unsigned int y, float cx, float cy, bool reverse)
{
    float animation_mult = 0.01 * (reverse ? -1.0 : 1.0) * animation_speed * (animation_direction == 0 ? 1 : -1);
    float color_mult = 0.01 * (reverse ? -1.0 : 1.0) * color_rotation_speed * (color_rotation_direction == 0 ? -1 : 1);

    double angle    = atan2(y - cy, x - cx) * 180 / 3.14159265359;
    double distance = sqrt(pow(cx - x, 2) + pow(cy - y, 2));
    float  value    = cos(animation_mult * distance / (0.1 * (float) spacing)  + progress);

    hsv_t hsv;
    hsv.value = pow((value + 1) * 0.5, (11 - thickness)) * 255;
    hsv.hue = abs((int)(angle + distance + progress * color_mult * color_rotation_speed) % 360);
    hsv.saturation = 255;

    return RGBColor(hsv2rgb(&hsv));
}

void Hypnotoad::LoadCustomSettings(json settings)
{
    if(settings.contains("animation_speed"))            animation_speed          = settings["animation_speed"];
    if(settings.contains("color_rotation_speed"))       color_rotation_speed     = settings["color_rotation_speed"];
    if(settings.contains("animation_direction"))        animation_direction      = settings["animation_direction"];
    if(settings.contains("color_rotation_direction"))   color_rotation_direction = settings["color_rotation_direction"];
    if(settings.contains("spacing"))                    spacing                  = settings["spacing"];
    if(settings.contains("thickness"))                  thickness                = settings["thickness"];
    if(settings.contains("cx"))                         cx_shift                 = settings["cx"];
    if(settings.contains("cy"))                         cy_shift                 = settings["cy"];

    ui->animation_speed->setValue(animation_speed);
    ui->color_rotation_speed->setValue(color_rotation_speed);
    ui->animation_direction->setCurrentIndex(animation_direction);
    ui->color_rotation_direction->setCurrentIndex(color_rotation_direction);
    ui->spacing->setValue(spacing);
    ui->thickness->setValue(thickness);
    ui->cx->setValue(cx_shift);
    ui->cy->setValue(cy_shift);
}

json Hypnotoad::SaveCustomSettings(json settings)
{
    settings["animation_speed"]          = animation_speed;
    settings["color_rotation_speed"]     = color_rotation_speed;
    settings["animation_direction"]      = animation_direction;
    settings["color_rotation_direction"] = color_rotation_direction;
    settings["spacing"]                  = spacing;
    settings["thickness"]                = thickness;
    settings["cx"]                       = cx_shift;
    settings["cy"]                       = cy_shift;

    return settings;
}

void Hypnotoad::on_animation_speed_valueChanged(int value)
{
    animation_speed = value;
}

void Hypnotoad::on_color_rotation_speed_valueChanged(int value)
{
    color_rotation_speed = value;
}

void Hypnotoad::on_animation_direction_currentIndexChanged(int value)
{
    animation_direction = value;
}

void Hypnotoad::on_color_rotation_direction_currentIndexChanged(int value)
{
    color_rotation_direction = value;
}

void Hypnotoad::on_spacing_valueChanged(int value)
{
    spacing = value;
}

void Hypnotoad::on_thickness_valueChanged(int value)
{
    thickness = value;
}

void Hypnotoad::on_cx_valueChanged(int value)
{
    cx_shift = value;
}

void Hypnotoad::on_cy_valueChanged(int value)
{
    cy_shift = value;
}

