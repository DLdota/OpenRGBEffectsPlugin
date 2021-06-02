#include "OpenRGBEffectsPlugin.h"
#include "OpenRGBEffectTab.h"
#include "EffectManager.h"

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

QWidget* OpenRGBEffectsPlugin::CreateGUI(QWidget*)
{
    RMPointer->WaitForDeviceDetection();

    OpenRGBEffectTab* ui = new OpenRGBEffectTab(nullptr);

    RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, ui);
    RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, ui);

    return ui;
}

void OpenRGBEffectsPlugin::DeviceListChangedCallback(void* o)
{
    // todo : investigate why this is called so many times (openrgb codebase)

    EffectManager::Get()->ClearAssignments();

    QMetaObject::invokeMethod((OpenRGBEffectTab *)o, "DeviceListChanged", Qt::QueuedConnection);
}
