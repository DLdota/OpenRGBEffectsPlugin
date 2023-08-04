#ifndef CONTROLLERZONE_H
#define CONTROLLERZONE_H

#include "ColorUtils.h"
#include "RGBController.h"
#include "json.hpp"

using json = nlohmann::json;

class ControllerZone
{

public:

    ControllerZone(RGBController* controller,
                   unsigned int zone_idx,
                   bool reverse,
                   int self_brightness,
                   bool is_segment,
                   int segment_idx = -1
            ):
        controller(controller),
        zone_idx(zone_idx),
        reverse(reverse),
        self_brightness(self_brightness),
        is_segment(is_segment),
        segment_idx(segment_idx)
    {};

    RGBController* controller;
    unsigned int zone_idx;
    bool reverse;
    unsigned int self_brightness;

    bool is_segment;
    int segment_idx;

    zone_type type()
    {
        return is_segment ? controller->zones[zone_idx].segments[segment_idx].type : controller->zones[zone_idx].type;
    }

    unsigned int startidx()
    {
        return is_segment ?
                   controller->zones[zone_idx].start_idx + controller->zones[zone_idx].segments[segment_idx].start_idx :
                   controller->zones[zone_idx].start_idx;
    }

    unsigned int zone_start_idx()
    {
        return is_segment ? controller->zones[zone_idx].segments[segment_idx].start_idx : 0;
    }

    unsigned int zone_stop_idx()
    {
        return zone_start_idx() + leds_count();
    }

    std::vector<RGBColor> colors()
    {
        std::vector<RGBColor> color_data;

        RGBColor* color_ptr = colors_ptr();

        for(unsigned int i = zone_start_idx(); i < zone_stop_idx(); i ++)
        {
            color_data.push_back(color_ptr[i]);
        }

        return color_data;
    }

    RGBColor* colors_ptr()
    {
        return controller->zones[zone_idx].colors;
    }

    unsigned int size()
    {
        return type() == ZONE_TYPE_MATRIX ? matrix_size() : leds_count();
    }

    unsigned int matrix_size()
    {
        return controller->zones[zone_idx].matrix_map->width *
                controller->zones[zone_idx].matrix_map->height;
    }

    unsigned int leds_count()
    {
        return is_segment ? controller->zones[zone_idx].segments[segment_idx].leds_count : controller->zones[zone_idx].leds_count;
    }

    unsigned int matrix_map_width()
    {
        return controller->zones[zone_idx].matrix_map->width;
    }

    unsigned int matrix_map_height()
    {
        return controller->zones[zone_idx].matrix_map->height;
    }

    unsigned int* map()
    {
        return controller->zones[zone_idx].matrix_map->map;
    }

    bool operator==(ControllerZone const & rhs) const
    {        
        return this->controller == rhs.controller && this->zone_idx == rhs.zone_idx && this->segment_idx == rhs.segment_idx;
    }

    bool operator<(ControllerZone const & rhs) const
    {
        if (is_segment)
        {
            return this->controller != rhs.controller || this->zone_idx != rhs.zone_idx || this->segment_idx != rhs.segment_idx;
        }
        else
        {
            // whatever
            return this->controller != rhs.controller || this->zone_idx != rhs.zone_idx;
        }
    }

    std::string display_name()
    {
        return controller->name + ": " +  controller->zones[zone_idx].name + (is_segment ? (" - " + controller->zones[zone_idx].segments[segment_idx].name) : "");
    }

    json to_json()
    {
        json j;
        j["zone_idx"] = zone_idx;
        j["reverse"] = reverse;
        j["self_brightness"] = self_brightness;
        j["name"] = controller->name;
        j["location"] = controller->location;
        j["serial"] = controller->serial;
        j["description"] = controller->description;
        j["version"] = controller->version;
        j["vendor"] = controller->vendor;
        j["is_segment"] = is_segment;
        j["segment_idx"] = segment_idx;
        return j;
    }

    void SetAllZoneLEDs(RGBColor color, int brightness, int temperature, int tint)
    {
        if(is_segment)
        {
            for(unsigned int i = 0; i < leds_count(); i ++)
            {
                SetLED(i, color, brightness, temperature, tint);
            }
        }
        else
        {
            controller->SetAllZoneLEDs(zone_idx, ColorUtils::apply_adjustments(color, (self_brightness / 100.f) * (brightness / 100.f), temperature, tint));
        }
    }

    void SetLED(int idx, RGBColor color, int brightness, int temperature, int tint)
    {       
        controller->SetLED(startidx() + idx, ColorUtils::apply_adjustments(color,  (self_brightness / 100.f) * (brightness / 100.f), temperature, tint));
    }

};

#endif // CONTROLLERZONE_H
