#include "Rain.h"
#include "hsv.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Rain);

Rain::Rain() : RGBEffect()
{
    EffectDetails.EffectName = "Rain";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Droplet effect";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed = 25;
    EffectDetails.MinSpeed = 1;

    EffectDetails.AllowOnlyFirst = true;
    EffectDetails.UserColors = 5;

    EffectDetails.MaxSlider2Val = 20;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Drops";

    EffectDetails.HasCustomWidgets = false;
    EffectDetails.HasCustomSettings = false;
}

void Rain::StepEffect(std::vector<ControllerZone> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i].start_idx();
        zone_type ZT = controller_zones[i].type();
        int leds_count = controller_zones[i].leds_count();
        bool reverse = controller_zones[i].reverse;

        unsigned int w;
        unsigned int h;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            w = 1;
            h = leds_count;

            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(i, 0, reverse ? leds_count - LedID - 1 : LedID);
                controller_zones[i].controller->SetLED(start_idx + LedID, color);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i].matrix_map_width();
            int rows = controller_zones[i].matrix_map_height();

            w = cols;
            h = rows;

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zones[i].controller->zones[controller_zones[i].zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    RGBColor color = GetColor(i, col_id, reverse ? rows - row_id - 1: row_id);
                    controller_zones[i].controller->SetLED(start_idx + LedID, color);
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
        float speed_mult = 1 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        Drop drop = {0.f, color, col, speed_mult};
        drops[controller_zone_index].push_back(drop);

    }
}

void Rain::RunDrops(unsigned int controller_zone_index)
{
    for(Drop& drop: drops[controller_zone_index])
    {
        drop.progress += drop.speed_mult * Speed / (float) FPS;
    }
}

void Rain::CleanDrops(unsigned int controller_zone_index, unsigned int h)
{
    for( std::vector<Drop>::iterator iter = drops[controller_zone_index].begin();
         iter != drops[controller_zone_index].end();)
    {
        if(iter->progress > h + 3)
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
        if(drop.col == x)
        {
            float distance = drop.progress - y;

            if(distance >= 0 && distance < 3)
            {
                float whole;
                float frac = std::modf(distance, &whole);

                if (whole == 2) // tail
                {
                    return ColorUtils::Enlight(drop.color, 1 - frac);
                }
                else if(whole == 1) // middle
                {
                    return drop.color;
                }
                else // head
                {
                    return ColorUtils::Enlight(drop.color, frac);
                }
            }
        }
    }

    return ColorUtils::OFF();
}

void Rain::ASelectionWasChanged(std::vector<ControllerZone> controller_zones)
{
    unsigned int zones_count = controller_zones.size();

    drops.resize(zones_count);

    for(unsigned int i = 0; i < zones_count; i++)
    {
        std::vector<Drop> zone_drops;
        drops[i] = zone_drops;
    }
}
