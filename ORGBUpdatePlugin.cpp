#include "ORGBUpdatePlugin.h"
#include "OpenRGBUpdateTab.h"
#include "Dependencies/ResourceManager.h"

bool ORGBPlugin::HasCustomIcon()
{
    return true;
}

bool ORGBPlugin::DarkTheme = false;

void ORGBPlugin::DefineNeeded()
{
    ORGBPlugin::NeededInfo.Needed   = {1,2};
    ORGBPlugin::NeededInfo.Settings = {{"Updates", "branch","fork"}};
}

QLabel* ORGBPlugin::TabLabel()
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

std::string ORGBPlugin::PluginName()
{
    return "Updates";
}

std::string ORGBPlugin::PluginDesc()
{
    return "An Auto Updating plugin for OpenRGB";
}

std::string ORGBPlugin::PluginLocal()
{
    return "InfoTab";
}

QWidget* ORGBPlugin::CreateGUI(QWidget *Parent)
{
    OpenRGBUpdateInfoPage *UpdatePage = NULL;
    ORGBPlugin::DarkTheme = NeededInfo.DarkTheme;
    /*-------------------------------------------------*\
    | Get prefered Branch/Fork from settings manager    |
    \*-------------------------------------------------*/
    std::vector<std::string> UpdateVars = {"master","CalcProgrammer1"};
    if ((ORGBPlugin::NeededInfo.Settings[0][0] != "NoSettingFound") && (ORGBPlugin::NeededInfo.Settings[0][0] != "Updates"))
    {
        UpdateVars[0] = ORGBPlugin::NeededInfo.Settings[0][0];
    }
    if ((ORGBPlugin::NeededInfo.Settings[0][1] != "NoSettingFound") && (ORGBPlugin::NeededInfo.Settings[0][1] != "branch"))
    {
        UpdateVars[1] = ORGBPlugin::NeededInfo.Settings[0][1];
    }
    qDebug() << QString().fromStdString(UpdateVars[0] + " " + UpdateVars[1]);
    UpdatePage = new OpenRGBUpdateInfoPage(UpdateVars,Parent);

    return UpdatePage;
}
