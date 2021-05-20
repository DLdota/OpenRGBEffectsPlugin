#include "Wavy.h"

REGISTER_EFFECT(Wavy);

Wavy::Wavy(QWidget *parent) :
    QWidget(parent),
    RGBEffect(),
    ui(new Ui::Wavy)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Wavy";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Alternate colors like waves";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 0;
    EffectDetails.MinSpeed     = 0;
    EffectDetails.UserColors   = 2;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    ui->oscillation_speed_slider->setMinimum(1);
    ui->oscillation_speed_slider->setMaximum(200);

    ui->wave_freq_slider->setMinimum(1);
    ui->wave_freq_slider->setMaximum(20);

    ui->wave_speed_slider->setMinimum(1);
    ui->wave_speed_slider->setMaximum(200);

    GenerateRandomColors();
}

Wavy::~Wavy()
{
    delete ui;
}

void Wavy::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Wavy::StepEffect(std::vector<ControllerZone> controller_zones)
{
    for(ControllerZone controller_zone : controller_zones)
    {
        zone_type ZT = controller_zone.type();

        if (ZT == ZONE_TYPE_LINEAR)
        {
            int start_idx = controller_zone.start_idx();
            int leds_count = controller_zone.leds_count();

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                controller_zone.controller->SetLED((start_idx + LedID), GetColor(LedID, leds_count));
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone.matrix_map_width();
            int rows = controller_zone.matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                RGBColor color = GetColor(col_id, cols);

                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zone.controller->zones[controller_zone.zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zone.controller->SetLED(LedID, color);
                }
            }
        }

    }


    if(Dir)
    {
        if(SineProgress < 1)
        {
            SineProgress += 0.01 * OscillationSpeed / FPS;
        }
        else
        {
            Dir = false;
            SineProgress -= 0.01 * OscillationSpeed / FPS;           
        }

    }
    else
    {
        if(SineProgress > -1)
        {
            SineProgress -= 0.01 * OscillationSpeed / FPS;

        }
        else
        {
            Dir = true;
            SineProgress += 0.01 * OscillationSpeed / FPS;            
        }
    }

    if(RandomColorsEnabled && SineProgress >= -0.01f && SineProgress <= 0.01f)
    {
        GenerateRandomColors();
    }

    SineProgress = std::min<float>(SineProgress, 1.0f);
    SineProgress = std::max<float>(SineProgress, -1.0f);

    if(WaveProgress < 100)
    {
        WaveProgress += 0.05 * WaveSpeed / FPS;
    } else {
        WaveProgress = 0.0;
    }
}


/*-----------------*\
| Custom Functions  |
\*-----------------*/
void Wavy::GenerateRandomColors()
{
    RandomColors.clear();

    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;

    RandomColors.push_back(RGBColor(ToRGBColor(r,g,b)));
    RandomColors.push_back(RGBColor(ToRGBColor((255-r),(255-g),(255-b))));
}

RGBColor Wavy::Interpolate(RGBColor color1, RGBColor color2, float fraction)
{
    unsigned char   r1 = RGBGetRValue(color1);
    unsigned char   r2 = RGBGetRValue(color2);

    unsigned char   g1 = RGBGetGValue(color1);
    unsigned char   g2 = RGBGetGValue(color2);

    unsigned char   b1 = RGBGetBValue(color1);
    unsigned char   b2 = RGBGetBValue(color2);

    return RGBColor(ToRGBColor(
                        (int) ((r2 - r1) * fraction + r1),
                        (int) ((g2 - g1) * fraction + g1),
                        (int) ((b2 - b1) * fraction + b1)
                        ));
}

RGBColor Wavy::GetColor(int i, int count)
{
    float pos = (i + (count * WaveProgress) / 100.0f)/count;
    float rad = (360.0f * pos) * PI / 180.0f;
    float wave_height = SineProgress * sin(WaveFrequency * rad);
    float h = 0.5 + wave_height/2;

    if(RandomColorsEnabled)
    {
        return Interpolate(RandomColors[0], RandomColors[1], h);
    }
    else
    {
        return Interpolate(UserColors[0], UserColors[1], h);
    }

}

void Wavy::LoadCustomSettings(json Settings)
{
    if (Settings.contains("WaveFrequency"))    WaveFrequency    = Settings["WaveFrequency"];
    if (Settings.contains("WaveSpeed"))        WaveSpeed        = Settings["WaveSpeed"];
    if (Settings.contains("OscillationSpeed")) OscillationSpeed = Settings["OscillationSpeed"];

    ui->wave_freq_slider->setValue(WaveFrequency);
    ui->wave_speed_slider->setValue(WaveSpeed);
    ui->oscillation_speed_slider->setValue(OscillationSpeed);
}

json Wavy::SaveCustomSettings(json Settings)
{
    Settings["WaveFrequency"]    = WaveFrequency;
    Settings["WaveSpeed"]        = WaveSpeed;
    Settings["OscillationSpeed"] = OscillationSpeed;
    return Settings;
}

void Wavy::on_wave_freq_slider_valueChanged(int NewVal)
{
    WaveFrequency = NewVal;
}

void Wavy::on_wave_speed_slider_valueChanged(int NewVal)
{
    WaveSpeed = NewVal;
}

void Wavy::on_oscillation_speed_slider_valueChanged(int value)
{
    OscillationSpeed = value;
}
