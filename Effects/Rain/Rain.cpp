#include "Rain.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Rain);

Rain::Rain(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Rain)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Rain";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Droplet effect";
    EffectDetails.IsReversable = true;
    EffectDetails.MinSpeed = 1;
    EffectDetails.MaxSpeed = 200;
    EffectDetails.Slider2Name   = "Drops";
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.MaxSlider2Val = 50;
    EffectDetails.UserColors = 5;
    EffectDetails.AllowOnlyFirst = true;
    EffectDetails.HasCustomSettings = true;

    SetSpeed(25);
    SetSlider2Val(20);
}

void Rain::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();
        bool reverse = controller_zones[i]->reverse;

        unsigned int w;
        unsigned int h;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            w = 1;
            h = leds_count;

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(i, 0, reverse ? leds_count - LedID - 1 : LedID);
                controller_zones[i]->SetLED(LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i]->matrix_map_width();
            int rows = controller_zones[i]->matrix_map_height();

            w = cols;
            h = rows;

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zones[i]->map()[((row_id * cols) + col_id)];
                    RGBColor color = GetColor(i, col_id, reverse ? rows - row_id - 1: row_id);
                    controller_zones[i]->SetLED(LedID, color, Brightness);
                }
            }
        }
        else
        {
            continue;
        }

        TriggerDrop(i, w);
        RunDrops(i);
        CleanDrops(i, h);
    }

}

void Rain::TriggerDrop(unsigned int controller_zone_index, unsigned int w)
{
    unsigned int max_drops = std::min(w, Slider2Val);

    if(drops[controller_zone_index].size() < max_drops && rand() % (2 + FPS / w) == 0)
    {
        RGBColor color;

        if(OnlyFirstColorEnabled)
        {
            color = UserColors[0];
        }
        else if(RandomColorsEnabled)
        {
            color = ColorUtils::RandomRGBColor();
        }
        else
        {
            color = UserColors[(rand() % UserColors.size())];
        }

        unsigned int col = rand() % w;
        float speed_mult = rand() % 3 + 1 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        Drop drop = {0.f, color, col, speed_mult, static_cast<unsigned int>(size)};
        drops[controller_zone_index].push_back(drop);

    }
}

void Rain::RunDrops(unsigned int controller_zone_index)
{
    for(Drop& drop: drops[controller_zone_index])
    {
        drop.progress += 0.5 * drop.speed_mult * Speed / (float) FPS;
    }
}

void Rain::CleanDrops(unsigned int controller_zone_index, unsigned int h)
{
    for( std::vector<Drop>::iterator iter = drops[controller_zone_index].begin();
         iter != drops[controller_zone_index].end();)
    {
        if(iter->progress > h + 3 * iter->size)
        {
            drops[controller_zone_index].erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

RGBColor Rain::GetColor(unsigned int controller_zone_index, unsigned int x, unsigned int y)
{
    for(Drop drop: drops[controller_zone_index])
    {
        if(drop.col >= x && drop.col <= (x + drop.size - 1))
        {
            float distance = drop.progress - y;

            // Vary trail length based on random speed_mult value
            int trailLength = (drop.speed_mult - 1) * ((drop.size / 2) + 1);

            if(distance >= 0 && distance <= drop.size + 1 + trailLength)
            {
                float whole;
                float frac = std::modf(distance, &whole);

                if (whole == 0) // head
                {
                    return ColorUtils::Enlight(drop.color, frac);
                }
                else if (whole > 0 && whole <= (drop.size)) // middle
                {
                    return drop.color;
                }
                else // tail
                {
                    return ColorUtils::Interpolate(0, drop.color, 0.75 / (whole - drop.size)); // fade to black
                }
            }
        }
    }

    return ColorUtils::OFF();
}

void Rain::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    unsigned int zones_count = controller_zones.size();

    drops.resize(zones_count);

    for(unsigned int i = 0; i < zones_count; i++)
    {
        std::vector<Drop> zone_drops;
        drops[i] = zone_drops;
    }
}

void Rain::on_size_valueChanged(int value)
{
    size = value;
}

Rain::~Rain()
{
    delete ui;
}

void Rain::LoadCustomSettings(json settings)
{
    if (settings.contains("size"))
    {
        ui->size->setValue(settings["size"]);
    }
}

json Rain::SaveCustomSettings()
{
    json settings;

    settings["size"] = size;

    return settings;
}
