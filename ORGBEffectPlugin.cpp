#include "ORGBEffectPlugin.h"
#include "ResourceManager.h"
#include "json.hpp"

bool ORGBPlugin::DarkTheme = false;

QLabel* TabLabel()
{
    QLabel* Label = new QLabel();
    Label->setText("Effects");
    return Label;
}

OpenRGBPluginInfo ORGBPlugin::Initialize(bool Dt, ResourceManager *RM)
{
    ORGBPlugin::PInfo.PluginName         = "Effects";
    ORGBPlugin::PInfo.PluginDescription  = "A Canned Effects Plugin";
    ORGBPlugin::PInfo.PluginLocation     = "TopTabBar";

    ORGBPlugin::PInfo.HasCustom   = true;

    ORGBPlugin::DarkTheme = Dt;

    ORGBPlugin::PInfo.PluginLabel = TabLabel();

    ORGBPlugin::RMPointer = RM;

    return ORGBPlugin::PInfo;
}

QWidget* ORGBPlugin::CreateGUI(QWidget *Parent)
{
    QWidget* BlankReturn = new QWidget();
    //OpenRGBUpdateInfoPage *UpdatePage = NULL;
    //UpdatePage = new OpenRGBUpdateInfoPage(Parent);

    return BlankReturn;
}
