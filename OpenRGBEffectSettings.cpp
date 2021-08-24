#include "OpenRGBEffectSettings.h"
#include <fstream>
#include <iostream>
#include "filesystem.h"
#include "OpenRGBEffectsPlugin.h"

unsigned int OpenRGBEffectSettings::version = 2;

const std::string OpenRGBEffectSettings::SettingsFolder     = "plugins/settings/";
const std::string OpenRGBEffectSettings::ProfilesFolder     = OpenRGBEffectSettings::SettingsFolder + "effect-profiles/";
const std::string OpenRGBEffectSettings::PatternsFolder     = OpenRGBEffectSettings::SettingsFolder + "effect-patterns/";
const std::string OpenRGBEffectSettings::SettingsFileName   = "EffectSettings.json";

void OpenRGBEffectSettings::SaveUserSettings(json Settings, std::string filename)
{
    if(!CreateSettingsDirectory())
    {
        printf("Cannot create settings directory.\n");
        return;
    }

    if(!CreateEffectProfilesDirectory())
    {
        printf("Cannot create profiles directory.\n");
        return;
    }

    std::ofstream EffectFile((OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + ProfilesFolder + filename), std::ios::out | std::ios::binary);

    if(EffectFile)
    {
        try{
            EffectFile << Settings.dump(4);
        }
        catch(const std::exception& e)
        {
            printf("Cannot write settings: %s\n", e.what());
        }

        EffectFile.close();
    }
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

    std::ifstream SFile(OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + ProfilesFolder + filename, std::ios::in | std::ios::binary);

    if(SFile)
    {
        try
        {
            SFile >> Settings;
            SFile.close();
        }
        catch(const std::exception& e)
        {
             printf("Cannot read settings: %s\n", e.what());
        }
    }

    return Settings;
}

bool OpenRGBEffectSettings::CreateSettingsDirectory()
{
    std::string directory = OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + SettingsFolder;

    if(filesystem::exists(directory))
    {
            return true;
    }

    return filesystem::create_directory(directory);
}

bool OpenRGBEffectSettings::CreateEffectPatternsDirectory(std::string effect_name)
{
    std::string directory = OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + PatternsFolder + "/" + effect_name;

    if(filesystem::exists(directory))
    {
        return true;
    }

    return filesystem::create_directories(directory);
}

bool OpenRGBEffectSettings::CreateEffectProfilesDirectory()
{
    std::string directory = OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + ProfilesFolder;

    if(filesystem::exists(directory))
    {
        return true;
    }

    return filesystem::create_directories(directory);
}

void OpenRGBEffectSettings::SaveEffectPattern(json content, std::string effect_name, std::string file_name)
{
    if(!CreateEffectPatternsDirectory(effect_name))
    {
        printf("Cannot create pattern directory for effect [%s].\n", effect_name.c_str());
        return;
    }

    std::ofstream PatternFile((OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + PatternsFolder + effect_name + "/" + file_name), std::ios::out | std::ios::binary);

    if(PatternFile)
    {
        try{
            PatternFile << content.dump(4);
        }
        catch(const std::exception& e)
        {
            printf("Cannot write pattern file: %s\n", e.what());
        }

        PatternFile.close();
    }
}

std::vector<std::string> OpenRGBEffectSettings::ListPattern(std::string effect_name)
{
    std::string path = OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + PatternsFolder + effect_name;

    std::vector<std::string> filenames;

    if(filesystem::exists(path))
    {
        for (filesystem::directory_entry entry : filesystem::directory_iterator(path))
        {
            filenames.push_back(entry.path().filename().u8string());
        }
    }

    return filenames;
}


std::vector<std::string> OpenRGBEffectSettings::ListProfiles()
{
    std::string path = OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + ProfilesFolder;

    std::vector<std::string> filenames;

    if(filesystem::exists(path))
    {
        for (filesystem::directory_entry entry : filesystem::directory_iterator(path))
        {
            filenames.push_back(entry.path().filename().u8string());
        }
    }

    return filenames;
}

json OpenRGBEffectSettings::LoadPattern(std::string effect_name, std::string file_name)
{
    json settings;

    std::ifstream SFile(OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + PatternsFolder + effect_name + "/" + file_name, std::ios::in | std::ios::binary);

    if(SFile)
    {
        try
        {
            SFile >> settings;
            SFile.close();
        }
        catch(const std::exception& e)
        {
             printf("Cannot read pattern file: %s\n", e.what());
        }
    }

    return settings;
}
