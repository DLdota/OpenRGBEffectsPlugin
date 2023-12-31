#include "Bubbles.h"
#include "ColorUtils.h"
#include "hsv.h"

REGISTER_EFFECT(Bubbles);

Bubbles::Bubbles(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Bubbles)
{
    ui->setupUi(this); 

    EffectDetails.EffectName = "Bubbles";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Bloop bloop";
    EffectDetails.UserColors   = 5;
    EffectDetails.HasCustomSettings = true;

    background = ColorUtils::OFF();
}


Bubbles::~Bubbles()
{
    delete ui;
}

void Bubbles::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(ControllerZone* controller_zone : controller_zones)
    {
        zone_type ZT = controller_zone->type();

        if (ZT == ZONE_TYPE_LINEAR || ZT == ZONE_TYPE_SINGLE)
        {            
            int leds_count = controller_zone->leds_count();

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                controller_zone->SetLED(LedID, GetColor(LedID, 0, leds_count, 1), Brightness, Temperature, Tint);
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
                    RGBColor color = GetColor(col_id, row_id, cols, rows);
                    int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                    controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
                }
            }
        }
    }

    for(unsigned int i = 0; i < bubbles.size(); i++)
    {
        bubbles[i] += 0.2 * speed_mult * speeds[i] / (float) FPS;
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

    RGBColor color;

    if(RandomColorsEnabled)
    {
        color = ColorUtils::RandomRGBColor();
    }
    else
    {
        color = UserColors[rand() % UserColors.size()];
    }

    colors.push_back(color);

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
    int color_idx = -1;
    int val = 0;

    for(unsigned int i = 0; i < bubbles.size(); i++)
    {
        double distance = sqrt(pow(w * centers[i].x() - x, 2) + pow(h * centers[i].y() - y, 2));
        double shallow = fabs(distance - bubbles[i]) / ( 0.1 * bubbles_thickness);
        double value = std::min<double>(255, 255 * (1 / pow(shallow, 2)));

        if(value > val)
        {
            color_idx = i;
            val = value;
        }
    }

    if(color_idx >= 0)
    {
        hsv_t final;

        rgb2hsv(colors[color_idx], &final);

        final.value = val;
        return ColorUtils::Screen(hsv2rgb(&final), background);
    }
    else
    {
        return background;
    }

}

void Bubbles::LoadCustomSettings(json Settings)
{
    if(Settings.contains("max_bubbles"))        ui->max_bubbles->setValue(Settings["max_bubbles"]);
    if(Settings.contains("rarity"))             ui->rarity->setValue(Settings["rarity"]);
    if(Settings.contains("speed_mult"))         ui->speed_mult->setValue(Settings["speed_mult"]);
    if(Settings.contains("max_expansion"))      ui->max_expansion->setValue(Settings["max_expansion"]);
    if(Settings.contains("bubbles_thickness"))  ui->bubbles_thickness->setValue(Settings["bubbles_thickness"]);
    if(Settings.contains("background"))         ui->background->SetRGBColor(Settings["background"]);
}

json Bubbles::SaveCustomSettings()
{
    json settings;

    settings["max_bubbles"]       = max_bubbles;
    settings["rarity"]            = rarity;
    settings["speed_mult"]        = speed_mult;
    settings["max_expansion"]     = max_expansion;
    settings["bubbles_thickness"] = bubbles_thickness;
    settings["background"]        = background;

    return settings;
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

void Bubbles::on_background_ColorSelected(QColor c)
{
    background = ColorUtils::fromQColor(c);
}
