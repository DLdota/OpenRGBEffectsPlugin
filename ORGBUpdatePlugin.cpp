#include "ORGBUpdatePlugin.h"
#include "OpenRGBUpdateTab.h"
#include "ResourceManager.h"
#include "dependencies/json/json.hpp"

std::string ORGBPlugin::fork      = "CalcProgrammer1";
std::string ORGBPlugin::branch    = "master";
bool        ORGBPlugin::DarkTheme = false;

QLabel* TabLabel()
{
    QString UpdateLabelTabString = "<html><table><tr><td width='30'><img src='";
    UpdateLabelTabString += ":/Update";
    if(ORGBPlugin::DarkTheme) UpdateLabelTabString += "_dark";
    UpdateLabelTabString += ".png' height='16' width='16'></td><td>Updates</td></tr></table></html>";

    QLabel *UpdateTabLabel = new QLabel();
    UpdateTabLabel->setText(UpdateLabelTabString);
    UpdateTabLabel->setIndent(20);
    if(ORGBPlugin::DarkTheme)
    {
        UpdateTabLabel->setGeometry(0, 25, 200, 50);
    }
    else
    {
        UpdateTabLabel->setGeometry(0, 0, 200, 25);
    }
    return UpdateTabLabel;
}

OpenRGBPluginInfo ORGBPlugin::DefineNeeded()
{
    ORGBPlugin::PInfo.PluginName  = "Updates";
    ORGBPlugin::PInfo.PluginDesc  = "A Plugin that bring auto updating to OpenRGB";
    ORGBPlugin::PInfo.PluginLoca  = "InfoTab";

    ORGBPlugin::PInfo.HasCustom   = true;

    return ORGBPlugin::PInfo;
}

OpenRGBPluginInfo ORGBPlugin::init(bool Dt, ResourceManagerInterface *RM)
{
    json Settings = RM->GetSettingsManager()->GetSettings("Updates");
    if (Settings.contains("branch"))
    {
        ORGBPlugin::branch = Settings["branch"];
    }
    if (Settings.contains("fork"))
    {
        ORGBPlugin::fork = Settings["fork"];
    }
    ORGBPlugin::DarkTheme = Dt;

    ORGBPlugin::PInfo.PluginLabel = TabLabel();

    return ORGBPlugin::PInfo;
}

QWidget* ORGBPlugin::CreateGUI(QWidget *Parent)
{
    OpenRGBUpdateInfoPage *UpdatePage = NULL;
    /*-------------------------------------------------*\
    | Get prefered Branch/Fork from settings manager    |
    \*-------------------------------------------------*/
    UpdatePage = new OpenRGBUpdateInfoPage(Parent);

    return UpdatePage;
}
