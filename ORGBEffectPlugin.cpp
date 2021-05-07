#include "ORGBEffectPlugin.h"
#include "OpenRGBEffectTab.h"

OpenRGBPluginInfo ORGBPlugin::Initialize(bool Dt, ResourceManager *RM)
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

QWidget* ORGBPlugin::CreateGUI(QWidget*)
{
    ORGBPlugin::RMPointer->WaitForDeviceDetection();
    return new OpenRGBEffectTab(nullptr);
}
