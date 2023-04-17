#include "OpenRGBEffectSettings.h"
#include <fstream>
#include <iostream>
#include "OpenRGBEffectsPlugin.h"

#include <QFile>
#include <QString>
#include <QDir>

unsigned int OpenRGBEffectSettings::version = 2;

GlobalSettingsStruct OpenRGBEffectSettings::globalSettings;

bool OpenRGBEffectSettings::WriteGlobalSettings()
{
    json j;

    j["fpscapture"]            = globalSettings.fpscapture;
    j["fps"]                   = globalSettings.fps;
    j["brightness"]            = globalSettings.brightness;
    j["temperature"]           = globalSettings.temperature;
    j["tint"]                  = globalSettings.tint;
    j["startup_profile"]       = globalSettings.startup_profile;
    j["hide_unsupported"]      = globalSettings.hide_unsupported;
    j["prefer_random"]         = globalSettings.prefer_random;
    j["prefered_colors"]       = globalSettings.prefered_colors;
    j["use_prefered_colors"]   = globalSettings.use_prefered_colors;
    j["audio_settings"]        = globalSettings.audio_settings;

    return write_json_to_file(SettingsFolder() / "EffectSettings.json", j);
}

void OpenRGBEffectSettings::LoadGlobalSettings()
{
    json j;

    std::ifstream file(SettingsFolder() / "EffectSettings.json");

    if(file)
    {
        try
        {
            file >> j;
            file.close();

            if(j.contains("fpscapture"))            globalSettings.fpscapture           =j["fpscapture"];
            if(j.contains("fps"))                   globalSettings.fps                  =j["fps"];
            if(j.contains("brightness"))            globalSettings.brightness           =j["brightness"];
            if(j.contains("temperature"))           globalSettings.temperature          =j["temperature"];
            if(j.contains("tint"))                  globalSettings.tint                 =j["tint"];
            if(j.contains("startup_profile"))       globalSettings.startup_profile      =j["startup_profile"];
            if(j.contains("hide_unsupported"))      globalSettings.hide_unsupported     =j["hide_unsupported"];
            if(j.contains("prefer_random"))         globalSettings.prefer_random        =j["prefer_random"];
            if(j.contains("use_prefered_colors"))   globalSettings.use_prefered_colors  =j["use_prefered_colors"];
            if(j.contains("audio_settings"))        globalSettings.audio_settings       =j["audio_settings"];


            if(j.contains("prefered_colors"))
            {
                for(unsigned int color : j["prefered_colors"])
                {
                    globalSettings.prefered_colors.push_back(color);
                }
            }
        }
        catch(const std::exception& e)
        {
             printf("[OpenRGBEffectsPlugin] Cannot read file: %s\n", e.what());
        }
    }
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
        if(globalSettings.startup_profile == filename)
        {
            globalSettings.startup_profile = "";
            WriteGlobalSettings();
        }

        return f.remove();
    }

    return false;
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

    return write_json_to_file(ProfilesFolder() / filename, j);
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

    return write_json_to_file(PatternsFolder() / effect_name / file_name, j);
}


bool OpenRGBEffectSettings::SaveShader(std::string content, std::string file_name)
{
    if(!CreateShadersDirectory())
    {
        return false;
    }

    return write_text_to_file(ShadersFolder() / file_name, content);
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

std::vector<std::string> OpenRGBEffectSettings::ListShaders()
{
    return list_files(ShadersFolder(), true);
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

bool OpenRGBEffectSettings::CreateShadersDirectory()
{
    return create_dir(ShadersFolder());
}

bool OpenRGBEffectSettings::write_json_to_file(filesystem::path file_name, json j)
{
    return write_text_to_file(file_name, j.dump(4));
}

bool OpenRGBEffectSettings::write_text_to_file(filesystem::path file_name, std::string content)
{
    std::ofstream file(file_name, std::ios::out | std::ios::binary);

    if(file)
    {
        try
        {
            file << content;
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

std::vector<std::string> OpenRGBEffectSettings::list_files(filesystem::path path, bool full_path)
{
    std::vector<std::string> filenames;

    QDir dir(QString::fromStdString(path.string()));

    if(dir.exists())
    {       
        for (const QString & entry : dir.entryList(QDir::Files))
        {
            std::string filename = entry.toStdString();

            if(full_path)
            {
                filenames.push_back(path / filename);
            }
            else
            {
                filenames.push_back(filename);
            }
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

filesystem::path OpenRGBEffectSettings::ShadersFolder()
{
    return SettingsFolder() / "effect-shaders";
}

filesystem::path OpenRGBEffectSettings::ProfilesFolder()
{
    return SettingsFolder() / "effect-profiles";
}

filesystem::path OpenRGBEffectSettings::PatternsFolder()
{
    return SettingsFolder() / "effect-patterns";
}
