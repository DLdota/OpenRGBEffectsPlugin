#include "Wavy.h"

EffectInfo Wavy::DefineEffectDetails()
{
    Wavy::EffectDetails.EffectName = "Wavy";
    Wavy::EffectDetails.EffectDescription = "Alternate colors like waves";

    Wavy::EffectDetails.IsReversable = false;
    Wavy::EffectDetails.MaxSpeed     = 0;
    Wavy::EffectDetails.MinSpeed     = 0;
    Wavy::EffectDetails.UserColors   = 2;

    Wavy::EffectDetails.MaxSlider2Val = 0;
    Wavy::EffectDetails.MinSlider2Val = 0;
    Wavy::EffectDetails.Slider2Name   = "";

    Wavy::EffectDetails.HasCustomWidgets = true;
    Wavy::EffectDetails.HasCustomSettings = true;

    return Wavy::EffectDetails;
}

void Wavy::SetWaveFrequency(int value)
{
    WaveFrequency = value;
}

void Wavy::SetWaveSpeed(int value)
{
    WaveSpeed = value;
}

void Wavy::SetOscillationSpeed(int value)
{
    OscillationSpeed = value;
}

void Wavy::DefineExtraOptions(QLayout* layout)
{
    ui = new WavyUi();
    layout->addWidget(ui);

    connect(ui, SIGNAL(OnWaveFrequencyChanged(int)), SLOT(SetWaveFrequency(int)));
    connect(ui, SIGNAL(OnWaveSpeedChanged(int)), SLOT(SetWaveSpeed(int)));
    connect(ui, SIGNAL(OnOscillationSpeedChanged(int)), SLOT(SetOscillationSpeed(int)));
}

void Wavy::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;

            if (ZT == ZONE_TYPE_LINEAR)
            {
                int SetLEDIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;
                int led_count = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;

                for (int LedID = 0; LedID < led_count; LedID++)
                {
                    Controllers[ControllerID].Controller->SetLED((SetLEDIndex+LedID), GetColor(LedID, led_count));
                }
            }

            else if (ZT == ZONE_TYPE_MATRIX)
            {
                int cols = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                int rows = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                for (int col_id = 0; col_id < cols; col_id++)
                {
                    RGBColor color = GetColor(col_id, cols);

                    for (int row_id = 0; row_id < rows; row_id++)
                    {
                        int LedID = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->map[((row_id * cols) + col_id)];
                        Controllers[ControllerID].Controller->SetLED(LedID, color);
                    }
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
            if(IsRandomColors)
            {
                GenerateRandomColors();
            }
        }
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

void Wavy::SetUserColors(std::vector<RGBColor> NewUserColors)
{
    UserColors = NewUserColors;
}

void Wavy::ToggleRandomColors(bool RandomEnabled)
{
    IsRandomColors = RandomEnabled;
}

RGBColor Wavy::GetColor(int i, int count)
{
    float pos = (i + (count * WaveProgress) / 100.0f)/count;
    float rad = (360.0f * pos) * PI / 180.0f;
    float wave_height = SineProgress * sin(WaveFrequency * rad);
    float h = 0.5 + wave_height/2;

    if(IsRandomColors)
    {
        return Interpolate(RandomColors[0], RandomColors[1], h);
    }
    else
    {
        return Interpolate(UserColors[0], UserColors[1], h);
    }

}

RGBColor Wavy::Interpolate(RGBColor color1, RGBColor color2, float fraction)
{
    unsigned char   r1 = RGBGetRValue(color1);
    unsigned char   r2 = RGBGetRValue(color2);

    unsigned char   g1 = RGBGetGValue(color1);
    unsigned char   g2 = RGBGetGValue(color2);

    unsigned char   b1 = RGBGetBValue(color1);
    unsigned char   b2 = RGBGetBValue(color2);

    RGBColor color = RGBColor(ToRGBColor(
                        (int) ((r2 - r1) * fraction + r1),
                        (int) ((g2 - g1) * fraction + g1),
                        (int) ((b2 - b1) * fraction + b1)
                        ));

    return color;
}

void Wavy::GenerateRandomColors()
{
    RandomColors.clear();
    RandomColors.push_back(RGBColor(ToRGBColor(rand() % 255,rand() % 255, rand() % 255)));
    RandomColors.push_back(RGBColor(ToRGBColor(rand() % 255,rand() % 255, rand() % 255)));
}

void Wavy::LoadCustomSettings(json Settings)
{
    if (Settings.contains("WaveFrequency"))    WaveFrequency    = Settings["WaveFrequency"];
    if (Settings.contains("WaveSpeed"))        WaveSpeed        = Settings["WaveSpeed"];
    if (Settings.contains("OscillationSpeed")) OscillationSpeed = Settings["OscillationSpeed"];

    ui->updateUi(WaveFrequency, WaveSpeed, OscillationSpeed);
    return;
}

json Wavy::SaveCustomSettings(json Settings)
{
    Settings["WaveFrequency"]    = WaveFrequency;
    Settings["WaveSpeed"]        = WaveSpeed;
    Settings["OscillationSpeed"] = OscillationSpeed;
    return Settings;
}
