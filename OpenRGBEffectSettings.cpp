#include "OpenRGBEffectSettings.h"


void OpenRGBEffectSettings::SaveUserSettings(json Settings)
{
    if(!CreateSettingsDirectory())
    {
        printf("Cannot create settings directory.\n");
        return;
    }

    std::ofstream EffectFile((ORGBPlugin::RMPointer->GetConfigurationDirectory() + SettingsFolder + SettingsFFileName), std::ios::out | std::ios::binary);

    if(EffectFile)
    {
        try{
            EffectFile << Settings.dump(4);
        }
        catch(std::exception e)
        {
            printf("Cannot write settings.\n");
        }
        EffectFile.close();
    }
}

json OpenRGBEffectSettings::LoadUserSettings()
{
    json Settings;

    std::ifstream SFile(ORGBPlugin::RMPointer->GetConfigurationDirectory() + SettingsFolder + SettingsFFileName, std::ios::in | std::ios::binary);

    if(SFile)
    {
        try
        {
            SFile >> Settings;
            SFile.close();
        }
        catch(std::exception e)
        {
             printf("Cannot read settings.\n");
        }
    }

    return Settings;
}

bool OpenRGBEffectSettings::CreateSettingsDirectory()
{
    std::string directory = ORGBPlugin::RMPointer->GetConfigurationDirectory() + SettingsFolder;

    if(std::filesystem::exists(directory))
    {
            return true;
    }

    return std::filesystem::create_directory(directory);
}
