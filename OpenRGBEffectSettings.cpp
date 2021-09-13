#include "OpenRGBEffectSettings.h"
#include <fstream>
#include <iostream>
#include "filesystem.h"
#include "OpenRGBEffectsPlugin.h"

unsigned int OpenRGBEffectSettings::version = 2;

bool OpenRGBEffectSettings::SaveUserSettings(json j, std::string filename)
{
    if(!CreateSettingsDirectory())
    {
        printf("Cannot create settings directory.\n");
        return false;
    }

    if(!CreateEffectProfilesDirectory())
    {
        printf("Cannot create profiles directory.\n");
        return false;
    }

    return write_file(ProfilesFolder() + folder_separator() + filename, j);
}

json OpenRGBEffectSettings::LoadUserSettings(std::string filename)
{
    json Settings;

    if(!CreateSettingsDirectory())
    {
        printf("Cannot create settings directory.\n");
        return Settings;
    }

    if(!CreateEffectProfilesDirectory())
    {
        printf("Cannot create profiles directory.\n");
        return Settings;
    }

    return load_json_file(ProfilesFolder() + folder_separator() + filename);
}

bool OpenRGBEffectSettings::SaveEffectPattern(json j, std::string effect_name, std::string file_name)
{
    if(!CreateEffectPatternsDirectory(effect_name))
    {
        printf("Cannot create pattern directory for effect [%s].\n", effect_name.c_str());
        return false;
    }

    return write_file(PatternsFolder() + folder_separator() + effect_name + folder_separator() + file_name, j);
}

json OpenRGBEffectSettings::LoadPattern(std::string effect_name, std::string file_name)
{
    return load_json_file(PatternsFolder() + folder_separator() + effect_name + folder_separator() + file_name);
}

std::vector<std::string> OpenRGBEffectSettings::ListPattern(std::string effect_name)
{
    return list_files(PatternsFolder() + folder_separator() + effect_name);
}

std::vector<std::string> OpenRGBEffectSettings::ListProfiles()
{
    return list_files(ProfilesFolder());
}

bool OpenRGBEffectSettings::CreateSettingsDirectory()
{
    return create_dir(SettingsFolder());
}

bool OpenRGBEffectSettings::CreateEffectPatternsDirectory(std::string effect_name)
{
    return create_dir(PatternsFolder() + folder_separator() + effect_name);
}

bool OpenRGBEffectSettings::CreateEffectProfilesDirectory()
{
    return create_dir(ProfilesFolder());
}

std::string OpenRGBEffectSettings::folder_separator()
{
#if defined(WIN32) || defined(_WIN32)
    return "\\";
#else
    return "/";
#endif
}

bool OpenRGBEffectSettings::write_file(std::string file_name, json j)
{
    std::ofstream file(file_name, std::ios::out | std::ios::binary);

    if(file)
    {
        try
        {
            file << j.dump(4);
            file.close();
        }
        catch(const std::exception& e)
        {
            printf("Cannot write file: %s\n", e.what());
            return false;
        }
    }

    return true;
}

json OpenRGBEffectSettings::load_json_file(std::string file_name)
{
    json j;

    std::ifstream file(file_name);

    if(file)
    {
        try
        {
            file >> j;
            file.close();
        }
        catch(const std::exception& e)
        {
             printf("Cannot read file: %s\n", e.what());
        }
    }

    return j;
}

std::vector<std::string> OpenRGBEffectSettings::list_files(std::string path)
{
    std::vector<std::string> filenames;

    if(filesystem::exists(path))
    {
        for (const auto & entry : filesystem::directory_iterator(path))
        {
            filenames.push_back(entry.path().filename().u8string());
        }
    }

    return filenames;
}

bool OpenRGBEffectSettings::create_dir(std::string directory)
{
    printf("Creating folder: %s\n", directory.c_str());

    if(filesystem::exists(directory))
    {
        printf("Already exists : %s\n", directory.c_str());
        return true;
    }

    return filesystem::create_directories(directory);
}


std::string OpenRGBEffectSettings::SettingsFolder()
{
    return OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + "plugins" + folder_separator() + "settings";
}

std::string OpenRGBEffectSettings::ProfilesFolder()
{
    return SettingsFolder() + folder_separator() + "effect-profiles";
}

std::string OpenRGBEffectSettings::PatternsFolder()
{
    return SettingsFolder() + folder_separator() + "effect-patterns";
}
