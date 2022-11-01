#ifndef CONTROLLERZONE_H
#define CONTROLLERZONE_H

#include "ColorUtils.h"
#include "RGBController.h"
#include "json.hpp"

using json = nlohmann::json;

class ControllerZone
{

public:

    ControllerZone(RGBController* controller, unsigned int zone_idx, bool reverse):
        controller(controller),
        zone_idx(zone_idx),
        reverse(reverse)
    {};

    RGBController* controller;
    unsigned int zone_idx;
    bool reverse;
    unsigned int self_brightness;

    zone_type type()
    {
        return controller->zones[zone_idx].type;
    }

    unsigned int start_idx()
    {
        return controller->zones[zone_idx].start_idx;
    }

    unsigned int size()
    {
        return type() == ZONE_TYPE_MATRIX ?
                    controller->zones[zone_idx].matrix_map->width *
                controller->zones[zone_idx].matrix_map->height : leds_count();
    }

    unsigned int matrix_size()
    {
        return controller->zones[zone_idx].matrix_map->width *
                controller->zones[zone_idx].matrix_map->height;
    }

    unsigned int leds_count()
    {
        return controller->zones[zone_idx].leds_count;
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
        return this->controller == rhs.controller && this->zone_idx == rhs.zone_idx;
    }

    bool operator<(ControllerZone const & rhs) const
    {
        // whatever
        return this->controller != rhs.controller || this->zone_idx != rhs.zone_idx;
    }

    std::string display_name()
    {
        return controller->name + " " +  std::to_string(zone_idx);
    }

    json to_json()
    {
        json j;
        j["zone_idx"] = zone_idx;
        j["reverse"] = reverse;
        j["name"] = controller->name;
        j["location"] = controller->location;
        j["serial"] = controller->serial;
        j["description"] = controller->description;
        j["version"] = controller->version;
        j["vendor"] = controller->vendor;
        return j;
    }

    void SetAllZoneLEDs(RGBColor color, int brightness)
    {
        controller->SetAllZoneLEDs(zone_idx, ColorUtils::apply_brightness(color, (self_brightness / 100.f) * (brightness / 100.f)));
    }

    void SetLED(int idx, RGBColor color, int brightness)
    {
         controller->SetLED(idx, ColorUtils::apply_brightness(color,  (self_brightness / 100.f) * (brightness / 100.f)));
    }

};

#endif // CONTROLLERZONE_H
