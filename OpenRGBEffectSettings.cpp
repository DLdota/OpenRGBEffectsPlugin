#include "OpenRGBEffectSettings.h"
#include <fstream>
#include <iostream>
#include <QFile>
#include <QString>
#include <QDir>
#include "OpenRGBEffectsPlugin.h"

unsigned int OpenRGBEffectSettings::version = 2;


bool OpenRGBEffectSettings::SetDefaultProfile(std::string filename)
{
    json j;
    j["default_profile"] = filename;

    return write_file(SettingsFolder() + folder_separator() + "EffectSettings.json", j);
}

bool OpenRGBEffectSettings::DeleteProfile(std::string filename)
{
    if(filename.empty())
    {
        return false;
    }

    std::string path = ProfilesFolder() + folder_separator() + filename;

    QFile f(QString::fromStdString(path));

    if (f.exists())
    {
        if(filename == DefaultProfile())
        {
            SetDefaultProfile("");
        }

        return f.remove();
    }

    return false;
}

std::string OpenRGBEffectSettings::DefaultProfile()
{
    json j;

    std::ifstream file(SettingsFolder() + folder_separator() + "EffectSettings.json");

    if(file)
    {
        try
        {
            file >> j;
            file.close();
        }
        catch(const std::exception& e)
        {
             printf("[OpenRGBEffectsPlugin] Cannot read file: %s\n", e.what());
        }
    }

    if(j.contains("default_profile"))
    {
        return j["default_profile"];
    }

    return "";
}

bool OpenRGBEffectSettings::SaveUserProfile(json j, std::string filename)
{
    if(!CreateSettingsDirectory())
    {
        return false;
    }

    if(!CreateEffectProfilesDirectory())
    {
        return false;
    }

    return write_file(ProfilesFolder() + folder_separator() + filename, j);
}

json OpenRGBEffectSettings::LoadUserProfile(std::string filename)
{
    json Settings;

    if(!CreateSettingsDirectory())
    {
        return Settings;
    }

    if(!CreateEffectProfilesDirectory())
    {
        return Settings;
    }

    return load_json_file(ProfilesFolder() + folder_separator() + filename);
}

bool OpenRGBEffectSettings::SaveEffectPattern(json j, std::string effect_name, std::string file_name)
{
    if(!CreateEffectPatternsDirectory(effect_name))
    {
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
            printf("[OpenRGBEffectsPlugin] Cannot write file: %s\n", e.what());
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
             printf("[OpenRGBEffectsPlugin] Cannot read file: %s\n", e.what());
        }
    }

    return j;
}

std::vector<std::string> OpenRGBEffectSettings::list_files(std::string path)
{
    std::vector<std::string> filenames;

    QDir dir(QString::fromStdString(path));

    if(dir.exists())
    {       
        for (const QString & entry : dir.entryList(QDir::Files))
        {
            filenames.push_back(entry.toStdString());
        }
    }

    // alphabetical sort
    std::sort(filenames.begin(), filenames.end());

    return filenames;
}

bool OpenRGBEffectSettings::create_dir(std::string directory)
{
    QDir dir(QString::fromStdString(directory));

    if(dir.exists())
    {
        return true;
    }

    return QDir().mkpath(dir.path());
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
