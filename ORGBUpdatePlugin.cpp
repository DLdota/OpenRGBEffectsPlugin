#include "ORGBUpdatePlugin.h"
#include "Dependencies/ResourceManager.h"

bool ORGBPlugin::DarkTheme = false;

bool ORGBPlugin::HasCustomIcon()
{
    return true;
}

QLabel* ORGBPlugin::TabLabel()
{
    QString UpdateLabelTabString = "<html><table><tr><td width='30'><img src='";
    UpdateLabelTabString += ":/Update";
    if(DarkTheme) UpdateLabelTabString += "_dark";
    UpdateLabelTabString += ".png' height='16' width='16'></td><td>Updates</td></tr></table></html>";

    QLabel *UpdateTabLabel = new QLabel();
    UpdateTabLabel->setText(UpdateLabelTabString);
    UpdateTabLabel->setIndent(20);
    if(DarkTheme)
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

QWidget* ORGBPlugin::CreateGUI(QWidget *Parent, ResourceManager *RM, bool DarkTheme)
{
    OpenRGBUpdateInfoPage *UpdatePage = NULL;

    ORGBPlugin::DarkTheme = DarkTheme;

    UpdatePage = new OpenRGBUpdateInfoPage(Parent);

    return UpdatePage;
}
