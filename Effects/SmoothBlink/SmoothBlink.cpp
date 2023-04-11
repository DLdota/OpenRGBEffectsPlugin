#include "SmoothBlink.h"
#include "ColorUtils.h"

REGISTER_EFFECT(SmoothBlink);

/*              N peaks              */
/*           /\    /\    ^           */
/*  <--I--> /  \  /  \   | S         */
/*  _______/ d  \/    \  |           */
/*               reset^              */
/*  <fade>       x --------->        */


SmoothBlink::SmoothBlink(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::SmoothBlink)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "SmoothBlink";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Create your own breathing sequences";
    EffectDetails.UserColors   = 2;
    EffectDetails.HasCustomSettings = true;

    random_color_1 = ColorUtils::RandomRGBColor();
    random_color_2 = ColorUtils::RandomRGBColor();
    next_color_1 = ColorUtils::RandomRGBColor();
    next_color_2 = ColorUtils::RandomRGBColor();

    ui->rendering->addItems({"Solid", "Circle"});

    Defaults();
}

SmoothBlink::~SmoothBlink()
{
    delete ui;
}

void SmoothBlink::Defaults()
{
    ui->interval->setValue(default_interval);
    ui->pulses->setValue(default_pulses);
    ui->pulse_duration->setValue(default_pulse_duration);
    ui->strength->setValue(default_strength);
    ui->rendering->setCurrentIndex(default_rendering);
    ui->cx->setValue(50);
    ui->cy->setValue(50);
}

void SmoothBlink::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    if(RandomColorsEnabled)
    {
        if(random_fade_timer <= 0.5 * interval)
        {
            current_color_1 = ColorUtils::Interpolate(random_color_1, next_color_1, random_fade_timer / (0.5 * interval));
            current_color_2 = ColorUtils::Interpolate(random_color_2, next_color_2, random_fade_timer / (0.5 * interval));
        }
        else
        {
            random_color_1 = next_color_1;
            random_color_2 = next_color_2;
        }
    }
    else
    {
        current_color_1 = UserColors[0];
        current_color_2 = UserColors[1];
    }

    pulses_total_duration = pulses * pulse_duration;
    total_effect_duration = interval + pulses_total_duration;

    float s = 0.5 + (1 - strength) * 0.5;

    if(time < interval)
    {
        value = 1.0;
    }
    else
    {
        float x = time - interval;
        float y = 0.5 * (1 + sin(2 * pulses / pulses_total_duration * x * pi - 0.5 * pi));
        value = y - (y - s) / s;
    }

    switch (rendering) {
    case Solid:
        HandleSolidRendering(controller_zones);
        break;

    case Circle:
        HandleCircleRendering(controller_zones);
        break;

    default: break;
    }

    time += 1.0 / (float) FPS;    
    random_fade_timer += 1.0 / (float) FPS;

    if(time >= total_effect_duration)
    {
        time = time - total_effect_duration;
        next_color_1 = ColorUtils::RandomRGBColor();
        next_color_2 = ColorUtils::RandomRGBColor();
        random_fade_timer = 0;
    }

}

void SmoothBlink::HandleSolidRendering(std::vector<ControllerZone*> controller_zones)
{
    RGBColor color = ColorUtils::Interpolate(current_color_1, current_color_2, value);

    for(ControllerZone* controller_zone : controller_zones)
    {
        controller_zone->SetAllZoneLEDs(color, Brightness);
    }
}

void SmoothBlink::HandleCircleRendering(std::vector<ControllerZone*> controller_zones)
{
    float cx_shift_mult = cx_shift / 100.f;
    float cy_shift_mult = cy_shift / 100.f;

    for(ControllerZone* controller_zone : controller_zones)
    {
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            float cx = (width - 1) * cx_shift_mult;
            float cy = (height - 1) * cy_shift_mult;

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(i, 0, cx, cy, width);
                controller_zone->SetLED(i, color, Brightness);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();

            float cx = (width - 1) * cx_shift_mult;
            float cy = (height - 1) * cy_shift_mult;

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    RGBColor color = GetColor(w, h, cx, cy, width + height);

                    unsigned int led_num = controller_zone->map()[h * width + w];
                    controller_zone->SetLED(led_num, color, Brightness);
                }
            }

        }
    }
}

RGBColor SmoothBlink::GetColor(unsigned int x, unsigned int y, double cx, double cy, unsigned int max_distance)
{
    if(value >= 1)
    {
        return current_color_2;
    }

    double distance = sqrt(pow(cx - x, 2) + pow(cy - y, 2));
    float distance_percent = distance / max_distance;

    return ColorUtils::Interpolate(current_color_1, current_color_2, std::min<double>(1,value + distance_percent));
}

void SmoothBlink::LoadCustomSettings(json settings)
{
    if(settings.contains("interval"))         interval       = settings["interval"];
    if(settings.contains("strength"))         strength       = settings["strength"];
    if(settings.contains("pulses"))           pulses         = settings["pulses"];
    if(settings.contains("pulse_duration"))   pulse_duration = settings["pulse_duration"];
    if(settings.contains("rendering"))        rendering      = settings["rendering"];
    if(settings.contains("cx"))               cx_shift       = settings["cx"];
    if(settings.contains("cy"))               cy_shift       = settings["cy"];

    ui->interval->setValue(interval);
    ui->strength->setValue(strength);
    ui->pulses->setValue(pulses);
    ui->pulse_duration->setValue(pulse_duration);
    ui->rendering->setCurrentIndex(rendering);
    ui->cx->setValue(cx_shift);
    ui->cy->setValue(cy_shift);
}

json SmoothBlink::SaveCustomSettings()
{
    json settings;

    settings["interval"]       = interval;
    settings["strength"]       = strength;
    settings["pulses"]         = pulses;
    settings["pulse_duration"] = pulse_duration;
    settings["rendering"]      = rendering;
    settings["cx"]             = cx_shift;
    settings["cy"]             = cy_shift;

    return settings;
}

void SmoothBlink::on_interval_valueChanged(double value)
{
    interval = value;
}

void SmoothBlink::on_pulses_valueChanged(int value)
{
    pulses = value;
}

void SmoothBlink::on_pulse_duration_valueChanged(double value)
{
    pulse_duration = value;
}

void SmoothBlink::on_strength_valueChanged(double value)
{
    strength = value;
}

void SmoothBlink::on_rendering_currentIndexChanged(int index)
{
    rendering = static_cast<PulseRendering>(index);
}

void SmoothBlink::on_cx_valueChanged(int value)
{
    cx_shift = value;
}

void SmoothBlink::on_cy_valueChanged(int value)
{
    cy_shift = value;
}


void SmoothBlink::on_defaults_clicked()
{
    Defaults();
}

