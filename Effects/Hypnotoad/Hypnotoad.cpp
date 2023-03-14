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
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    ui->color_mode->addItems({"Rainbow", "Custom"});
    ui->color_rotation_direction->addItems({"Clockwise","Counter-clockwise"});
    ui->animation_direction->addItems({"To the inside","To the outside"});

    ui->colors_picker->setVisible(false);
    ui->gradient->setVisible(false);
    gradient = QImage(100, 1, QImage::Format_RGB32);

    GenerateGradient();

    SetSpeed(50);
}

Hypnotoad::~Hypnotoad()
{
    delete ui;
}

void Hypnotoad::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    float cx_shift_mult = cx_shift / 100.f;
    float cy_shift_mult = cy_shift / 100.f;

    for(ControllerZone* controller_zone : controller_zones)
    {
        unsigned int start_idx = controller_zone->start_idx();
        bool reverse = controller_zone->reverse;
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            float cx = (width-1) * cx_shift_mult;
            float cy = height * cy_shift_mult;

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(i, 0, cx, cy, reverse);
                controller_zone->SetLED(start_idx + i, color, Brightness);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();
            unsigned int * map = controller_zone->map();

            float cx = (width-1) * cx_shift_mult;
            float cy = (height-1) * cy_shift_mult;

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    RGBColor color = GetColor(w, h, cx, cy, reverse);

                    unsigned int led_num = map[h * width + w];
                    controller_zone->SetLED(start_idx + led_num, color, Brightness);
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

    if(color_mode == COLOR_MODE_RAINBOW)
    {
        hsv.value = pow((value + 1) * 0.5, (11 - thickness)) * 255;
        hsv.hue = abs((int)(angle + distance + progress * color_mult * color_rotation_speed) % 360);
        hsv.saturation = 255;

        return RGBColor(hsv2rgb(&hsv));
    }
    else if(color_mode == COLOR_MODE_CUSTOM)
    {
        float v = abs((int)(angle + distance + progress * color_mult * color_rotation_speed) % 360) / 360.0;
        QColor c = gradient.pixelColor(v * 100.0, 0);
        RGBColor color = ColorUtils::fromQColor(c);

        return ColorUtils::Enlight(color, pow((value + 1) * 0.5, (11 - thickness)));
    }
    else
    {
        return ColorUtils::OFF();
    }

}

void Hypnotoad::LoadCustomSettings(json settings)
{
    if(settings.contains("color_mode"))                 color_mode               = settings["color_mode"];
    if(settings.contains("animation_speed"))            animation_speed          = settings["animation_speed"];
    if(settings.contains("color_rotation_speed"))       color_rotation_speed     = settings["color_rotation_speed"];
    if(settings.contains("animation_direction"))        animation_direction      = settings["animation_direction"];
    if(settings.contains("color_rotation_direction"))   color_rotation_direction = settings["color_rotation_direction"];
    if(settings.contains("spacing"))                    spacing                  = settings["spacing"];
    if(settings.contains("thickness"))                  thickness                = settings["thickness"];
    if(settings.contains("cx"))                         cx_shift                 = settings["cx"];
    if(settings.contains("cy"))                         cy_shift                 = settings["cy"];

    if(settings.contains("colors"))                     ui->colors_picker->SetColors(settings["colors"]);

    ui->color_mode->setCurrentIndex(color_mode);
    ui->animation_speed->setValue(animation_speed);
    ui->color_rotation_speed->setValue(color_rotation_speed);
    ui->animation_direction->setCurrentIndex(animation_direction);
    ui->color_rotation_direction->setCurrentIndex(color_rotation_direction);
    ui->spacing->setValue(spacing);
    ui->thickness->setValue(thickness);
    ui->cx->setValue(cx_shift);
    ui->cy->setValue(cy_shift);
}

json Hypnotoad::SaveCustomSettings()
{
    json settings;

    settings["color_mode"]               = color_mode;
    settings["colors"]                   = ui->colors_picker->Colors();
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

void Hypnotoad::on_color_mode_currentIndexChanged(int value)
{
    color_mode = value;
    ui->colors_picker->setVisible(value == COLOR_MODE_CUSTOM);
    ui->gradient->setVisible(value == COLOR_MODE_CUSTOM);
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

void Hypnotoad::on_colors_picker_ColorsChanged()
{
    GenerateGradient();
}

void Hypnotoad::GenerateGradient()
{
    QPointF start_point(0,0);
    QPointF end_point(100,0);

    QLinearGradient grad(start_point, end_point);

    grad.setSpread(QGradient::Spread::PadSpread);

    std::vector<RGBColor> colors = ui->colors_picker->Colors();
    float step = 1.f / (colors.size() -1);
    float start = 0.f;

    QGradientStops stops;

    for(RGBColor& color: colors)
    {
        QGradientStop stop(start, ColorUtils::toQColor(color));
        start += step;
        stops.append(stop);
    }

    grad.setStops(stops);

    QBrush brush(grad);
    QRectF rect(0, 0, 100, 1);
    QPainter painter(&gradient);
    painter.fillRect(rect, brush);


    ui->gradient->setPixmap(QPixmap::fromImage(gradient));
}

