#include "ORGBEffectPlugin.h"
#include "Dependencies/ResourceManager.h"

std::string ORGBPlugin::PluginName() const
{
    return "Effects";
}

std::string ORGBPlugin::PluginDesc() const
{
    return "A plugin based effect manager for OpenRGB";
}

std::string ORGBPlugin::PluginLocal() const
{
    return "TopTabBar";
}

QWidget* ORGBPlugin::CreateGUI(QWidget *Parent, ResourceManager *RM) const
{
    //Ui::PluginUi *PluginPage = new Ui::PluginUi(Parent);

    //QWidget *EffectInfoTab = CreateInfoTab();
    //EffectBar->setTabButton( (EffectBar->count() - 1) , QTabBar::LeftSide,EffectInfoTab);
    return new QWidget(Parent);
}

QWidget* ORGBPlugin::CreateInfoTab() const
{
    QWidget *Base = new QWidget();
    QLabel *InfoTabLabel = new QLabel(Base);
    InfoTabLabel->setText("ORGB Effects Plugin\nA Plugin by CoffeeIsLife");
    return Base;
}
