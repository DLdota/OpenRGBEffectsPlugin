#ifndef OPENRGBEFFECTSETTINGS_H
#define OPENRGBEFFECTSETTINGS_H


#include <fstream>
#include <iostream>
#include <string>
#include "filesystem.h"
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

    static const std::string SettingsFolder;
    static const std::string SettingsFileName;
};

#endif // OPENRGBEFFECTSETTINGS_H
