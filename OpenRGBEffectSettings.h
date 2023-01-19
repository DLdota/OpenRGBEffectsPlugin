#ifndef OPENRGBEFFECTSETTINGS_H
#define OPENRGBEFFECTSETTINGS_H

#include <fstream>
#include <iostream>
#include <string>
#include "json.hpp"
#include "filesystem.h"

using json = nlohmann::json;

class OpenRGBEffectSettings
{

public:    
    static unsigned int version;

    static bool DeleteProfile(std::string);
    static bool SetDefaultProfile(std::string);
    static std::string DefaultProfile();

    static bool SaveUserProfile(json, std::string);
    static json LoadUserProfile(std::string);
    static std::vector<std::string> ListProfiles();

    static bool SaveEffectPattern(json, std::string, std::string);
    static std::vector<std::string> ListPattern(std::string);
    static json LoadPattern(std::string, std::string);

    static filesystem::path PatternsFolder();

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
