#ifndef OPENRGBEFFECTSETTINGS_H
#define OPENRGBEFFECTSETTINGS_H

#include "RGBController.h"
#include "json.hpp"

using json = nlohmann::json;

class OpenRGBEffectSettings
{

public:
    static void SaveUserSettings(json UserSettings);
    static json LoadUserSettings();

    static unsigned int version;

private:
    static bool CreateSettingsDirectory();

    static inline const std::string SettingsFolder     = "/plugins/settings/";
    static inline const std::string SettingsFileName   = "EffectSettings.json";

};

#endif // OPENRGBEFFECTSETTINGS_H
