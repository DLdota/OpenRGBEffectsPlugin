#include "OpenRGBEffectsPlugin.h"
#include "OpenRGBEffectTab.h"
#include "EffectManager.h"
#include "EffectList.h"
#include <QSystemTrayIcon>
#include <QMenu>

bool OpenRGBEffectsPlugin::DarkTheme = false;
ResourceManager* OpenRGBEffectsPlugin::RMPointer = nullptr;

OpenRGBPluginInfo OpenRGBEffectsPlugin::Initialize(bool Dt, ResourceManager *RM)
{
    DarkTheme = Dt;
    RMPointer = RM;

    PInfo.PluginName         = "Effects";
    PInfo.PluginDescription  = "A Canned Effects Plugin";
    PInfo.PluginLocation     = "TopTabBar";
    PInfo.HasCustom          = true;
    PInfo.PluginLabel        = new QLabel("Effects");

    return PInfo;
}

QWidget* OpenRGBEffectsPlugin::CreateGUI(QWidget* parent)
{
    printf("[OpenRGBEffectsPlugin] version %s (%s), build date %s\n", VERSION_STRING, GIT_COMMIT_ID, GIT_COMMIT_DATE);

    std::map<std::string, std::function<RGBEffect*()>>::iterator it;

    for (it = EffectList::effects_construtors.begin(); it != EffectList::effects_construtors.end(); it++)
    {
        printf("[OpenRGBEffectsPlugin] %s effect registered\n", it->first.c_str());
    }

    RMPointer->WaitForDeviceDetection();

    OpenRGBEffectTab* ui = new OpenRGBEffectTab(nullptr);

    RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, ui);
    RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, ui);

    QSystemTrayIcon* trayIcon = parent->findChild<QSystemTrayIcon*>();

    if(trayIcon)
    {
        QMenu* pluginsMenu = new QMenu("Effects", parent);

        QAction* actionStartAll = new QAction("Start all effects", this);

        connect(actionStartAll, &QAction::triggered, [ui](){
            ui->StartAll();
        });

        pluginsMenu->addAction(actionStartAll);

        QAction* actionStopAll = new QAction("Stop all effects", this);

        connect(actionStopAll, &QAction::triggered, [ui](){
            ui->StopAll();
        });

        pluginsMenu->addAction(actionStopAll);

        trayIcon->contextMenu()->addMenu(pluginsMenu);
    }


    return ui;
}

void OpenRGBEffectsPlugin::DeviceListChangedCallback(void* o)
{
    // todo : investigate why this is called so many times (openrgb codebase)

    EffectManager::Get()->ClearAssignments();

    QMetaObject::invokeMethod((OpenRGBEffectTab *)o, "DeviceListChanged", Qt::QueuedConnection);
}
