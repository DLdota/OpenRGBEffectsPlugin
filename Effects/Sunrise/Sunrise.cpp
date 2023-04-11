#include "Sunrise.h"
#include <QPainter>

REGISTER_EFFECT(Sunrise);

Sunrise::Sunrise(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Sunrise)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Sunrise";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Sunrise/Sunset effect";
    EffectDetails.MaxSpeed     = 20;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 4;
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    UserColors.resize(4);
    UserColors[0] = ColorUtils::fromQColor(Qt::white);
    UserColors[1] = ColorUtils::fromQColor(Qt::yellow);
    UserColors[2] = ColorUtils::fromQColor(Qt::red);
    UserColors[3] = ColorUtils::fromQColor(Qt::black);

    SetSpeed(10);
}

Sunrise::~Sunrise()
{
    delete ui;
}

void Sunrise::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    QGradientStops stops;

    QLinearGradient grad;

    float first_stop = std::min<float>(0.01 * max_intensity, pow(progress, 0.1 * intensity_speed));
    float second_stop = first_stop + (1 - first_stop) * 0.5;

    stops << QGradientStop(0, ColorUtils::toQColor(UserColors[0]));
    stops << QGradientStop(first_stop, ColorUtils::toQColor(UserColors[1]));
    stops << QGradientStop(second_stop, ColorUtils::toQColor(UserColors[2]));
    stops << QGradientStop(1, ColorUtils::toQColor(UserColors[3]));

    grad = QLinearGradient(QPointF(0,0), QPointF(100,0));
    grad.setSpread(QGradient::PadSpread);
    grad.setStops(stops);

    QBrush brush = QBrush(grad);
    QRectF rect(0, 0, 100, 1);

    QPainter painter(&image);
    painter.fillRect(rect, brush);

    for(ControllerZone* controller_zone : controller_zones)
    {
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(i, 0, width, height);
                controller_zone->SetLED(i, color, Brightness, Temperature, Tint);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    RGBColor color = GetColor(w, h, width, height);
                    unsigned int led_num = controller_zone->map()[h * width + w];
                    controller_zone->SetLED(led_num, color, Brightness, Temperature, Tint);
                }
            }

        }
    }

    time +=  0.1 * (float) Speed / (float) FPS;

    if(run_once)
    {
        progress = std::min<float>(1, time);
        y_shift = 0;
    }
    else
    {
        progress = 0.5 * (1 + sin(time));

        y_shift = -1 + 2 * progress;
    }
}


RGBColor Sunrise::GetColor(float x, float y, float w, float h)
{
    float real_radius = 0.01 * radius * w * pow(progress, 0.1 * grow_speed);

    float hx = 0.5 * (w-1);
    float hy = 0.5 * (h-1);

    double distance = sqrt(pow(x - hx, 2) + pow(y + (motion ? hy * y_shift : 0) - hy , 2));
    double percent = std::max<float>(0, std::min<float>(1, distance / real_radius));
    return ColorUtils::fromQColor(image.pixelColor(99 * percent, 0));
}

void Sunrise::on_max_intensity_valueChanged(int value)
{
    max_intensity = value;
}

void Sunrise::on_intensity_speed_valueChanged(int value)
{
    intensity_speed = value;
}

void Sunrise::on_radius_valueChanged(int value)
{
    radius = value;
}

void Sunrise::on_grow_speed_valueChanged(int value)
{
    grow_speed = value;
}

void Sunrise::on_motion_stateChanged(int value)
{
    motion = value;
}

void Sunrise::on_run_once_stateChanged(int value)
{
    run_once = value;
}


void Sunrise::LoadCustomSettings(json settings)
{
    if(settings.contains("max_intensity"))      ui->max_intensity->setValue(settings["max_intensity"]);
    if(settings.contains("intensity_speed"))    ui->intensity_speed->setValue(settings["intensity_speed"]);
    if(settings.contains("motion"))             ui->motion->setCheckState(settings["motion"] ? Qt::Checked:Qt::Unchecked);
    if(settings.contains("radius"))             ui->radius->setValue(settings["radius"]);
    if(settings.contains("grow_speed"))         ui->grow_speed->setValue(settings["grow_speed"]);
    if(settings.contains("run_once"))           ui->run_once->setCheckState(settings["run_once"] ? Qt::Checked:Qt::Unchecked);
}

json Sunrise::SaveCustomSettings()
{
    json settings;

    settings["max_intensity"]   = max_intensity;
    settings["intensity_speed"] = intensity_speed;
    settings["motion"]          = motion;
    settings["radius"]          = radius;
    settings["grow_speed"]      = grow_speed;
    settings["run_once"]        = run_once;

    return settings;
}
