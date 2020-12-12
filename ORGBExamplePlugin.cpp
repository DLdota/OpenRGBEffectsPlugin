#include "ORGBExamplePlugin.h"

#include <QDebug>

std::string ORGBPlugin::PluginName() const
{
    return "ExamplePlugin";
}

std::string ORGBPlugin::PluginDesc() const
{
    return "This is an Example plugin for OpenRGB";
}

std::string ORGBPlugin::PluginLocal() const
{
    return "InfoTab";
}

QWidget* ORGBPlugin::CreateGUI(QWidget *Parent) const
{
    QWidget *ORGBExamplePage = new QWidget(Parent);
    QLabel *ORGBExampleLabel = new QLabel(ORGBExamplePage);
    ORGBExampleLabel->setText("This is an example page added by plugins");
    return ORGBExamplePage;
}
