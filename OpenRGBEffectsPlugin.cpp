#include "OpenRGBEffectsPlugin.h"
#include "EffectManager.h"
#include "EffectList.h"
#include "OpenRGBEffectSettings.h"
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

    printf("[OpenRGBEffectsPlugin] %lu effects registered\n", EffectList::effects_construtors.size());

    RMPointer->WaitForDeviceDetection();

    OpenRGBEffectTab* ui = new OpenRGBEffectTab();

    this->ui = ui;

    RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, ui);
    RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, ui);

    return ui;
}

QMenu* OpenRGBEffectsPlugin::GetTrayMenu()
{
    QMenu* pluginsMenu = new QMenu("Effects", ui);

    QMenu* profilesMenu = new QMenu("Profiles", pluginsMenu);

    pluginsMenu->setObjectName("OpenRGBEffectsPlugin::profilesMenu");

    // START ALL EFFECTS

    QAction* actionStartAll = new QAction("Start all effects", ui);

    connect(actionStartAll, &QAction::triggered, [=](){
        QMetaObject::invokeMethod(ui, "StartAll", Qt::QueuedConnection);
    });

    actionStartAll->setObjectName("OpenRGBEffectsPlugin::Action::StartAll");
    actionStartAll->setProperty("OpenRGBEffectsPlugin::ActionTitle", actionStartAll->text());
    actionStartAll->setParent(pluginsMenu);

    pluginsMenu->addAction(actionStartAll);

    // STOP ALL EFFECTS

    QAction* actionStopAll = new QAction("Stop all effects", ui);

    connect(actionStopAll, &QAction::triggered, [=](){
        QMetaObject::invokeMethod(ui, "StopAll", Qt::QueuedConnection);
    });

    actionStopAll->setObjectName("OpenRGBEffectsPlugin::Action::StopAll");
    actionStopAll->setProperty("OpenRGBEffectsPlugin::ActionTitle", actionStopAll->text());

    pluginsMenu->addAction(actionStopAll);

    // PROFILES SHORTUCTS

    connect(ui, &OpenRGBEffectTab::ProfileListUpdated, [=](){
        profilesMenu->clear();

        std::vector<std::string> profiles = OpenRGBEffectSettings::ListProfiles();

        for(const std::string& profile: profiles)
        {
            QString profile_name = QString::fromStdString(profile);

            QAction* profileAction = new QAction(profile_name, ui);

            connect(profileAction, &QAction::triggered, [=](){
                QMetaObject::invokeMethod(ui, "LoadProfile", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(profile)));
            });

            profileAction->setObjectName("OpenRGBEffectsPlugin::Action::Profile::"+profile_name);
            profileAction->setProperty("OpenRGBEffectsPlugin::ActionTitle", "Load profile: " + profile_name);
            profilesMenu->addAction(profileAction);
        }
    });

    pluginsMenu->addMenu(profilesMenu);

    return(pluginsMenu);
}

void OpenRGBEffectsPlugin::Unload()
{
    printf("[OpenRGBEffectsPlugin] Unloading\n");

    ui->StopAll();

    RMPointer->UnregisterDeviceListChangeCallback(DeviceListChangedCallback, ui);
    RMPointer->UnregisterDetectionProgressCallback(DeviceListChangedCallback, ui);
}

void OpenRGBEffectsPlugin::DeviceListChangedCallback(void* o)
{
    // todo : investigate why this is called so many times (openrgb codebase)

    EffectManager::Get()->ClearAssignments();

    QMetaObject::invokeMethod((OpenRGBEffectTab *)o, "DeviceListChanged", Qt::QueuedConnection);
}
