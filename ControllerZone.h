#ifndef CONTROLLERZONE_H
#define CONTROLLERZONE_H

#include "RGBController.h"
#include "json.hpp"

using json = nlohmann::json;

struct ControllerZone
{
    RGBController* controller;
    unsigned int zone_idx;
    bool reverse;

    zone_type type()
    {
        return controller->zones[zone_idx].type;
    }

    unsigned int start_idx()
    {
        return controller->zones[zone_idx].start_idx;
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

    bool operator==(ControllerZone const & rhs) const {
        return this->controller == rhs.controller && this->zone_idx == rhs.zone_idx;
    }

    bool operator<(ControllerZone const & rhs) const {
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
};

#endif // CONTROLLERZONE_H
