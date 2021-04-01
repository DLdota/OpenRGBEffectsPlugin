#ifndef OPENRGBEFFECTSETTINGS_H
#define OPENRGBEFFECTSETTINGS_H

#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

#include "RGBController.h"
#include "json.hpp"

using json = nlohmann::json;

struct EffectSettings
{
    std::vector<RGBColor> UserColors;
    int                   SpeedSliderVal;
    int                   Slider2Val;
};

class OpenRGBEffectSettings
{

public:
    static void SaveUserSettings(json UserSettings);
    static json LoadUserSettings();

private:
    static bool CreateSettingsDirectory();

    static inline const std::string SettingsFolder     = "/plugins/settings/";
    static inline const std::string SettingsFileName   = "EffectSettings.json";
};

#endif // OPENRGBEFFECTSETTINGS_H
