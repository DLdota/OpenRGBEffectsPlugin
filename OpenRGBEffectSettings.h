#ifndef OPENRGBEFFECTSETTINGS_H
#define OPENRGBEFFECTSETTINGS_H

#include <fstream>
#include <iostream>
#include <string>
#include "json.hpp"
#include "filesystem.h"
#include "RGBController.h"

struct GlobalSettingsStruct
{
    int  fpscapture = 60;
    int  brightness = 100;
    int  fps = 60;
    bool hide_unsupported = false;
    bool prefer_random = false;
    bool use_prefered_colors = false;
    std::string startup_profile;
    std::vector<RGBColor> prefered_colors;
};

using json = nlohmann::json;

class OpenRGBEffectSettings
{

public:    
    static unsigned int version;

    static bool WriteGlobalSettings();
    static void LoadGlobalSettings();

    static bool DeleteProfile(std::string);
    static bool SaveUserProfile(json, std::string);
    static json LoadUserProfile(std::string);
    static std::vector<std::string> ListProfiles();

    static bool SaveEffectPattern(json, std::string, std::string);
    static std::vector<std::string> ListPattern(std::string);
    static json LoadPattern(std::string, std::string);

    static filesystem::path PatternsFolder();

    static GlobalSettingsStruct globalSettings;

private:
    static bool CreateSettingsDirectory();
    static bool CreateEffectProfilesDirectory();
    static bool CreateEffectPatternsDirectory(std::string);

    static filesystem::path SettingsFolder();
    static filesystem::path ProfilesFolder();

    static bool create_dir(filesystem::path);
    static std::vector<std::string> list_files(filesystem::path);
    static json load_json_file(filesystem::path);
    static bool write_file(filesystem::path, json);

};

#endif // OPENRGBEFFECTSETTINGS_H
