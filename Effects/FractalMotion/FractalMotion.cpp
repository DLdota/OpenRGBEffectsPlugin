#include "FractalMotion.h"

REGISTER_EFFECT(FractalMotion);

FractalMotion::FractalMotion(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::FractalMotion)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "FractalMotion";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Psychedelic wave";
    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 20;
    EffectDetails.UserColors   = 1;
    EffectDetails.HasCustomSettings = true;

    ui->background->SetRGBColor(ColorUtils::OFF());

    random_color = ColorUtils::RandomRGBColor();
    next_random_color = ColorUtils::RandomRGBColor();

    SetSpeed(50);
    Defaults();
}

FractalMotion::~FractalMotion()
{
    delete ui;
}


void FractalMotion::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void FractalMotion::StepEffect(std::vector<ControllerZone*> controller_zones)
{

    for(ControllerZone* controller_zone: controller_zones)
    {
        int start_idx = controller_zone->start_idx();
        zone_type ZT = controller_zone->type();
        int leds_count = controller_zone->leds_count();

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0, 1);
                controller_zone->SetLED(start_idx + LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(col_id, row_id, rows);
                    int LedID = controller_zone->controller->zones[controller_zone->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zone->SetLED(start_idx + LedID, color, Brightness);
                }
            }
        }
    }

    float delta = (float) Speed / (float) FPS;

    if(random_tick >= 1)
    {
        random_color = next_random_color;
        next_random_color = ColorUtils::RandomRGBColor();
        random_tick = 0;
    }

    random_tick += 0.005 * delta;
    progress += 0.1 * delta;
}

RGBColor FractalMotion::GetColor(unsigned int x, unsigned int ly, unsigned int h)
{
    float f = frequency * 0.01;
    float y = sin(x * f);
    float t = 0.01*(-progress*Speed);
    y += sin(x*f*freq_m1 + t)*freq_m2;
    y += sin(x*f*freq_m3 + t*freq_m4)*freq_m5;
    y += sin(x*f*freq_m6 + t*freq_m7)*freq_m8;
    y += sin(x*f*freq_m9+ t*freq_m10)*freq_m11;
    y *= 0.1 * amplitude*freq_m12;

    y = (1 + y) * 0.5 * h;

    float distance = fabs(y - ly);

    if(distance > thickness)
    {
        return background;
    }

    RGBColor color;

    if(RandomColorsEnabled)
    {
        color = ColorUtils::Interpolate(random_color, next_random_color, std::min<float>(1.f, random_tick));
    }
    else
    {
        color = UserColors[0];
    }

    return ColorUtils::Interpolate(color, background, distance / thickness);
}

void FractalMotion::LoadCustomSettings(json settings)
{
    if(settings.contains("background"))
    {
        background = settings["background"];
        ui->background->SetRGBColor(background);
    }

    if(settings.contains("thickness"))  thickness  = settings["thickness"];
    if(settings.contains("amplitude"))  amplitude  = settings["amplitude"];
    if(settings.contains("frequency"))  frequency  = settings["frequency"];
    if(settings.contains("freq_m1"))  freq_m1  = settings["freq_m1"];
    if(settings.contains("freq_m2"))  freq_m2  = settings["freq_m2"];
    if(settings.contains("freq_m3"))  freq_m3  = settings["freq_m3"];
    if(settings.contains("freq_m4"))  freq_m4  = settings["freq_m4"];
    if(settings.contains("freq_m5"))  freq_m5  = settings["freq_m5"];
    if(settings.contains("freq_m6"))  freq_m6  = settings["freq_m6"];
    if(settings.contains("freq_m7"))  freq_m7  = settings["freq_m7"];
    if(settings.contains("freq_m8"))  freq_m8  = settings["freq_m8"];
    if(settings.contains("freq_m9"))  freq_m9  = settings["freq_m9"];
    if(settings.contains("freq_m10")) freq_m10 = settings["freq_m10"];
    if(settings.contains("freq_m11")) freq_m11 = settings["freq_m11"];
    if(settings.contains("freq_m12")) freq_m12 = settings["freq_m12"];

    ui->thickness->setValue(thickness);
    ui->amplitude->setValue(amplitude * val_mult);
    ui->frequency->setValue(frequency * val_mult);
    ui->freq_m1->setValue(freq_m1 * val_mult);
    ui->freq_m2->setValue(freq_m2 * val_mult);
    ui->freq_m3->setValue(freq_m3 * val_mult);
    ui->freq_m4->setValue(freq_m4 * val_mult);
    ui->freq_m5->setValue(freq_m5 * val_mult);
    ui->freq_m6->setValue(freq_m6 * val_mult);
    ui->freq_m7->setValue(freq_m7 * val_mult);
    ui->freq_m8->setValue(freq_m8 * val_mult);
    ui->freq_m9->setValue(freq_m9 * val_mult);
    ui->freq_m10->setValue(freq_m10 * val_mult);
    ui->freq_m11->setValue(freq_m11 * val_mult);
    ui->freq_m12->setValue(freq_m12 * val_mult);

}

json FractalMotion::SaveCustomSettings(json settings)
{
    settings["background"]  = background;
    settings["thickness"]     = thickness;
    settings["amplitude"]     = amplitude;
    settings["frequency"]     = frequency;
    settings["freq_m1"]     = freq_m1;
    settings["freq_m2"]     = freq_m2;
    settings["freq_m3"]     = freq_m3;
    settings["freq_m4"]     = freq_m4;
    settings["freq_m5"]     = freq_m5;
    settings["freq_m6"]     = freq_m6;
    settings["freq_m7"]     = freq_m7;
    settings["freq_m8"]     = freq_m8;
    settings["freq_m9"]     = freq_m9;
    settings["freq_m10"]    = freq_m10;
    settings["freq_m11"]    = freq_m11;
    settings["freq_m12"]    = freq_m12;

    return settings;
}

void FractalMotion::on_background_ColorSelected(QColor c)
{
    background = ColorUtils::fromQColor(c);
}

void FractalMotion:: on_thickness_valueChanged(int value)
{
    thickness = value;
}

void FractalMotion:: on_amplitude_valueChanged(int value)
{
    amplitude = value * inv_val_mult;
}

void FractalMotion:: on_frequency_valueChanged(int value)
{
    frequency = value * inv_val_mult;
}

void FractalMotion::on_freq_m1_valueChanged(int value)
{
    freq_m1 = value * inv_val_mult;
}

void FractalMotion::on_freq_m2_valueChanged(int value)
{
    freq_m2 = value * inv_val_mult;
}

void FractalMotion::on_freq_m3_valueChanged(int value)
{
    freq_m3 = value * inv_val_mult;
}

void FractalMotion::on_freq_m4_valueChanged(int value)
{
    freq_m4 = value * inv_val_mult;
}

void FractalMotion::on_freq_m5_valueChanged(int value)
{
    freq_m5 = value * inv_val_mult;
}

void FractalMotion::on_freq_m6_valueChanged(int value)
{
    freq_m6 = value * inv_val_mult;
}

void FractalMotion::on_freq_m7_valueChanged(int value)
{
    freq_m7 = value * inv_val_mult;
}

void FractalMotion::on_freq_m8_valueChanged(int value)
{
    freq_m8 = value * inv_val_mult;
}

void FractalMotion::on_freq_m9_valueChanged(int value)
{
    freq_m9 = value * inv_val_mult;
}

void FractalMotion::on_freq_m10_valueChanged(int value)
{
    freq_m10 = value * inv_val_mult;
}

void FractalMotion::on_freq_m11_valueChanged(int value)
{
    freq_m11 = value * inv_val_mult;
}

void FractalMotion::on_freq_m12_valueChanged(int value)
{
    freq_m12 = value * inv_val_mult;
}

void FractalMotion::on_defaults_clicked()
{
    Defaults();
}

void FractalMotion::Defaults()
{
    ui->thickness->setValue(default_thickness);
    ui->frequency->setValue(default_frequency * val_mult);
    ui->amplitude->setValue(default_amplitude * val_mult);

    ui->freq_m1->setValue(default_freq_m1 * val_mult);
    ui->freq_m2->setValue(default_freq_m2 * val_mult);
    ui->freq_m3->setValue(default_freq_m3 * val_mult);
    ui->freq_m4->setValue(default_freq_m4 * val_mult);
    ui->freq_m5->setValue(default_freq_m5 * val_mult);
    ui->freq_m6->setValue(default_freq_m6 * val_mult);
    ui->freq_m7->setValue(default_freq_m7 * val_mult);
    ui->freq_m8->setValue(default_freq_m8 * val_mult);
    ui->freq_m9->setValue(default_freq_m9 * val_mult);
    ui->freq_m10->setValue(default_freq_m10 * val_mult);
    ui->freq_m11->setValue(default_freq_m11 * val_mult);
    ui->freq_m12->setValue(default_freq_m12 * val_mult);
}
