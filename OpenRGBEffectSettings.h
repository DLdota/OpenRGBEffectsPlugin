#ifndef OPENRGBEFFECTSETTINGS_H
#define OPENRGBEFFECTSETTINGS_H


#include <fstream>
#include <iostream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

class OpenRGBEffectSettings
{

public:
    static void SaveUserSettings(json UserSettings);
    static json LoadUserSettings();

    static void SaveEffectPattern(json, std::string, std::string);
    static std::vector<std::string> ListPattern(std::string);
    static json LoadPattern(std::string, std::string);

    static unsigned int version;

private:
    static bool CreateSettingsDirectory();
    static bool CreateEffectPatternsDirectory(std::string);

    static const std::string SettingsFolder;
    static const std::string PatternsFolder;
    static const std::string SettingsFileName;
};

#endif // OPENRGBEFFECTSETTINGS_H
