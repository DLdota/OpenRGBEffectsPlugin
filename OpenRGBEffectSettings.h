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

    static std::string PatternsFolder();

private:
    static bool CreateSettingsDirectory();
    static bool CreateEffectProfilesDirectory();
    static bool CreateEffectPatternsDirectory(std::string);

    static std::string folder_separator();
    static std::string SettingsFolder();
    static std::string ProfilesFolder();

    static bool create_dir(std::string);
    static std::vector<std::string> list_files(std::string);
    static json load_json_file(std::string);
    static bool write_file(std::string, json);
};

#endif // OPENRGBEFFECTSETTINGS_H
