#include "OpenRGBEffectSettings.h"
#include <fstream>
#include <iostream>
#include "filesystem.h"
#include "OpenRGBEffectsPlugin.h"

unsigned int OpenRGBEffectSettings::version = 2;

const std::string OpenRGBEffectSettings::SettingsFolder     = "/plugins/settings/";
const std::string OpenRGBEffectSettings::SettingsFileName   = "EffectSettings.json";

void OpenRGBEffectSettings::SaveUserSettings(json Settings)
{
    if(!CreateSettingsDirectory())
    {
        printf("Cannot create settings directory.\n");
        return;
    }

    std::ofstream EffectFile((OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + SettingsFolder + SettingsFileName), std::ios::out | std::ios::binary);

    if(EffectFile)
    {
        try{
            EffectFile << Settings.dump(4);
        }
        catch(const std::exception&)
        {
            printf("Cannot write settings.\n");
        }
        EffectFile.close();
    }
}

json OpenRGBEffectSettings::LoadUserSettings()
{
    json Settings;

    std::ifstream SFile(OpenRGBEffectsPlugin::RMPointer->GetConfigurationDirectory() + SettingsFolder + SettingsFileName, std::ios::in | std::ios::binary);

    if(SFile)
    {
        try
        {
            SFile >> Settings;
            SFile.close();
        }
        catch(const std::exception&)
        {
             printf("Cannot read settings.\n");
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
