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

    return write_file(SettingsFolder() / "EffectSettings.json", j);
}

bool OpenRGBEffectSettings::DeleteProfile(std::string filename)
{
    if(filename.empty())
    {
        return false;
    }

    filesystem::path path = ProfilesFolder() / filename;

    QFile f(QString::fromStdString(path.string()));

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

    std::ifstream file(SettingsFolder() / "EffectSettings.json");

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

    return write_file(ProfilesFolder() / filename, j);
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

    return load_json_file(ProfilesFolder() / filename);
}

bool OpenRGBEffectSettings::SaveEffectPattern(json j, std::string effect_name, std::string file_name)
{
    if(!CreateEffectPatternsDirectory(effect_name))
    {
        return false;
    }

    return write_file(PatternsFolder() / effect_name / file_name, j);
}

json OpenRGBEffectSettings::LoadPattern(std::string effect_name, std::string file_name)
{
    return load_json_file(PatternsFolder() / effect_name / file_name);
}

std::vector<std::string> OpenRGBEffectSettings::ListPattern(std::string effect_name)
{
    return list_files(PatternsFolder() / effect_name);
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
    return create_dir(PatternsFolder() / effect_name);
}

bool OpenRGBEffectSettings::CreateEffectProfilesDirectory()
{
    return create_dir(ProfilesFolder());
}

bool OpenRGBEffectSettings::write_file(filesystem::path file_name, json j)
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

json OpenRGBEffectSettings::load_json_file(filesystem::path file_name)
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

std::vector<std::string> OpenRGBEffectSettings::list_files(filesystem::path path)
{
    std::vector<std::string> filenames;

    QDir dir(QString::fromStdString(path.string()));

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

bool OpenRGBEffectSettings::create_dir(filesystem::path directory)
{
    QDir dir(QString::fromStdString(directory.string()));

    if(dir.exists())
    {
        return true;
    }

    return QDir().mkpath(dir.path());
}


filesystem::path OpenRGBEffectSettings::SettingsFolder()
{
    return OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() / "plugins" / "settings";
}

filesystem::path OpenRGBEffectSettings::ProfilesFolder()
{
    return SettingsFolder() / "effect-profiles";
}

filesystem::path OpenRGBEffectSettings::PatternsFolder()
{
    return SettingsFolder() / "effect-patterns";
}
