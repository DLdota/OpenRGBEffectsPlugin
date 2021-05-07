#include "Bubbles.h"
#include "hsv.h"

Bubbles::Bubbles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Bubbles)
{
    ui->setupUi(this); 
}


Bubbles::~Bubbles()
{
    delete ui;
}

EffectInfo Bubbles::DefineEffectDetails()
{
    EffectDetails.EffectName = "Bubbles";
    EffectDetails.EffectDescription = "Bloop bloop";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 0;
    EffectDetails.MinSpeed     = 0;
    EffectDetails.UserColors   = 1;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    return EffectDetails;
}

void Bubbles::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Bubbles::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;

            if (ZT == ZONE_TYPE_LINEAR || ZT == ZONE_TYPE_SINGLE)
            {
                int SetLEDIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;
                int led_count = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;

                for (int LedID = 0; LedID < led_count; LedID++)
                {
                    Controllers[ControllerID].Controller->SetLED((SetLEDIndex+LedID), GetColor(LedID, 0, led_count, 1));
                }
            }

            else if (ZT == ZONE_TYPE_MATRIX)
            {
                int cols = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                int rows = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;

                for (int col_id = 0; col_id < cols; col_id++)
                {
                    for (int row_id = 0; row_id < rows; row_id++)
                    {
                        RGBColor color = GetColor(col_id, row_id, cols, rows);
                        int LedID = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->map[((row_id * cols) + col_id)];
                        Controllers[ControllerID].Controller->SetLED(LedID, color);
                    }
                }
            }

        }
    }


    for(unsigned int i = 0; i < bubbles.size(); i++)
    {
        bubbles[i] += speed_mult * speeds[i] / (float) FPS;
    }


    if(rand() % rarity == 0 && bubbles.size() < max_bubbles)
    {
        InitBubble();
    }

    Cleanup();
}

void Bubbles::InitBubble()
{
    bubbles.push_back(0);

    double speed = 1 + 10 * (((double)rand() / RAND_MAX) - 1) *(-1);
    speeds.push_back(speed);

    int hue;

    if(random_enabled)
    {
        hue = 360 * (((double)rand() / RAND_MAX) - 1) *(-1);
    }
    else
    {
        hsv_t hsv;
        rgb2hsv(UserColors[0], &hsv);
        hue = hsv.hue;
    }

    colors.push_back(hue);

    double x = (((double)rand() / RAND_MAX) - 1) *(-1);
    double y = (((double)rand() / RAND_MAX) - 1) *(-1);
    centers.push_back(QPointF(x, y));
}

void Bubbles::Cleanup()
{
    for(unsigned int i = 0; i < bubbles.size(); i++)
    {
        if(bubbles[i] > max_expansion)
        {
            bubbles.erase(bubbles.begin() + i);
            centers.erase(centers.begin() + i);
            speeds.erase(speeds.begin() + i);
            colors.erase(colors.begin() + i);
        }
    }
}

RGBColor Bubbles::GetColor(int x, int y, int w, int h)
{
    std::vector<hsv_t> hsvs;

    for(unsigned int i = 0; i < bubbles.size(); i++)
    {
        double distance = sqrt(pow(w * centers[i].x() - x, 2) + pow(h * centers[i].y() - y, 2));

        double shallow = abs(distance - bubbles[i]) / ( 0.1 * bubbles_thickness);

        hsv_t hsv;
        hsv.hue = colors[i];
        hsv.saturation = 255;
        hsv.value = std::min<double>(255, 255 * (1 / pow(shallow, 2)));

        hsvs.push_back(hsv);
    }

    // todo interpolate colors

    int value = 0;
    int hue = 0;

    for(hsv_t hsv: hsvs)
    {
        if(hsv.value > value)
        {
            value = hsv.value;
            hue = hsv.hue;
        }
    }

    hsv_t final;
    final.hue = hue;
    final.value = value;
    final.saturation = 255;

    return RGBColor(hsv2rgb(&final));
}

void Bubbles::LoadCustomSettings(json Settings)
{
    if(Settings.contains("max_bubbles"))        max_bubbles       = Settings["max_bubbles"];
    if(Settings.contains("rarity"))             rarity            = Settings["rarity"];
    if(Settings.contains("speed_mult"))         speed_mult        = Settings["speed_mult"];
    if(Settings.contains("max_expansion"))      max_expansion     = Settings["max_expansion"];
    if(Settings.contains("bubbles_thickness"))  bubbles_thickness = Settings["bubbles_thickness"];

    ui->max_bubbles->setValue(max_bubbles);
    ui->rarity->setValue(rarity);
    ui->speed_mult->setValue(speed_mult);
    ui->max_expansion->setValue(max_expansion);
    ui->bubbles_thickness->setValue(bubbles_thickness);
}

json Bubbles::SaveCustomSettings(json Settings)
{
    Settings["max_bubbles"]       = max_bubbles;
    Settings["rarity"]            = rarity;
    Settings["speed_mult"]        = speed_mult;
    Settings["max_expansion"]     = max_expansion;
    Settings["bubbles_thickness"] = bubbles_thickness;
    return Settings;
}

void Bubbles::SetUserColors(std::vector<RGBColor> NewUserColors)
{
    UserColors = NewUserColors;
}

void Bubbles::ToggleRandomColors(bool value)
{
    random_enabled = value;
}

void Bubbles::on_max_bubbles_valueChanged(int value)
{
    max_bubbles = value;
}

void Bubbles::on_rarity_valueChanged(int value)
{
    rarity = value;
}

void Bubbles::on_max_expansion_valueChanged(int value)
{
    max_expansion = value;
}

void Bubbles::on_bubbles_thickness_valueChanged(int value)
{
    bubbles_thickness = value;
}

void Bubbles::on_speed_mult_valueChanged(int value)
{
    speed_mult = value;
}

