#include "OpenRGBEffectsPlugin.h"
#include "EffectManager.h"
#include "EffectList.h"
#include "NetworkServer.h"
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
    OpenRGBPluginInfo info = OpenRGBEffectsPlugin::GetPluginInfo();
    NetworkPlugin net_plugin;
    net_plugin.name = info.Name;
    net_plugin.description = info.Description;
    net_plugin.protocol_version = 1;
    net_plugin.version = info.Version;
    net_plugin.callback = OpenRGBEffectsPlugin::HandleSDK;
    net_plugin.callback_arg = (void*)this;
    RMPointer->GetServer()->RegisterPlugin(net_plugin);
    OpenRGBEffectSettings::LoadGlobalSettings();
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
    OpenRGBPluginInfo info = OpenRGBEffectsPlugin::GetPluginInfo();
    RMPointer->GetServer()->UnregisterPlugin(info.Name);
}

void OpenRGBEffectsPlugin::DeviceListChangedCallback(void* o)
{
    // todo : investigate why this is called so many times (openrgb codebase)

    EffectManager::Get()->ClearAssignments();

    QMetaObject::invokeMethod((OpenRGBEffectTab *)o, "DeviceListChanged", Qt::QueuedConnection);
}

unsigned char* OpenRGBEffectsPlugin::HandleSDK(void * instance, unsigned int pkt_id, unsigned char* data, unsigned int* data_size)
{
    OpenRGBEffectsPlugin* plugin = (OpenRGBEffectsPlugin*)instance;
    unsigned char* data_out = nullptr;

    switch (pkt_id)
    {
        case NET_PACKET_ID_REQUEST_EFFECT_LIST:
            data_out = plugin->ui->GetEffectListDescription(data_size);
            break;
        case NET_PACKET_ID_START_EFFECT:
            {
                unsigned short name_len;
                memcpy(&name_len, &data[0], sizeof(name_len));
                char* name = (char *)&data[sizeof(unsigned short)];
                plugin->ui->SetEffectState(std::string(name), true);
            }
            break;
        case NET_PACKET_ID_STOP_EFFECT:
            {
                unsigned short name_len;
                memcpy(&name_len, &data[0], sizeof(name_len));
                char* name = (char *)&data[sizeof(unsigned short)];
                plugin->ui->SetEffectState(std::string(name), false);
            }
            break;

    }
    return data_out;
}
