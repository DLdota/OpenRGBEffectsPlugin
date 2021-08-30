#include "OpenRGBEffectsPlugin.h"
#include "EffectManager.h"
#include "EffectList.h"
#include <QSystemTrayIcon>
#include <QMenu>

bool OpenRGBEffectsPlugin::DarkTheme = false;
ResourceManager* OpenRGBEffectsPlugin::RMPointer = nullptr;

OpenRGBPluginInfo OpenRGBEffectsPlugin::GetPluginInfo()
{
    OpenRGBPluginInfo info;

    info.Name           = "OpenRGB Effects Plugin";
    info.Description    = "Provides a variety of custom effects";
    info.Version        = VERSION_STRING;
    info.Commit         = GIT_COMMIT_ID;
    info.URL            = "https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin";

    info.Label          = "Effects";
    info.Location       = OPENRGB_PLUGIN_LOCATION_TOP;

    info.Icon.load(":/OpenRGBEffectsPlugin.png");

    return(info);
}

unsigned int OpenRGBEffectsPlugin::GetPluginAPIVersion()
{
    return(OPENRGB_PLUGIN_API_VERSION);
}

void OpenRGBEffectsPlugin::Load(bool Dt, ResourceManager *RM)
{
    DarkTheme = Dt;
    RMPointer = RM;
}

QWidget* OpenRGBEffectsPlugin::GetWidget()
{
    printf("[OpenRGBEffectsPlugin] version %s (%s), build date %s\n", VERSION_STRING, GIT_COMMIT_ID, GIT_COMMIT_DATE);

    std::map<std::string, std::function<RGBEffect*()>>::iterator it;

    for (it = EffectList::effects_construtors.begin(); it != EffectList::effects_construtors.end(); it++)
    {
        printf("[OpenRGBEffectsPlugin] %s effect registered\n", it->first.c_str());
    }

    RMPointer->WaitForDeviceDetection();

    OpenRGBEffectTab* ui = new OpenRGBEffectTab(nullptr);

    this->ui = ui;

    RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, ui);
    RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, ui);

    return ui;
}

QMenu* OpenRGBEffectsPlugin::GetTrayMenu()
{
    QMenu* pluginsMenu = new QMenu("Effects", nullptr);

    QAction* actionStartAll = new QAction("Start all effects", this);

    OpenRGBEffectTab* ui = this->ui;

    connect(actionStartAll, &QAction::triggered, [ui](){
        ui->StartAll();
    });

    pluginsMenu->addAction(actionStartAll);

    QAction* actionStopAll = new QAction("Stop all effects", this);

    connect(actionStopAll, &QAction::triggered, [ui](){
        ui->StopAll();
    });

    pluginsMenu->addAction(actionStopAll);

    return(pluginsMenu);
}

void OpenRGBEffectsPlugin::Unload()
{
    ui->StopAll();
}

void OpenRGBEffectsPlugin::DeviceListChangedCallback(void* o)
{
    // todo : investigate why this is called so many times (openrgb codebase)

    EffectManager::Get()->ClearAssignments();

    QMetaObject::invokeMethod((OpenRGBEffectTab *)o, "DeviceListChanged", Qt::QueuedConnection);
}
