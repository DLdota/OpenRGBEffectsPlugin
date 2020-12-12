#pragma once

#include "ORGBExamplePluginInterface.h"

#include <QObject>
#include <QString>
#include <QtPlugin>
#include "QWidget"
#include "QLabel"

class ORGBPlugin : public QObject, public ORGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ORGBPluginInterface_IID)
    Q_INTERFACES(ORGBPluginInterface)

public:
    ~ORGBPlugin() override {}

    std::string PluginName() const override;
    std::string PluginDesc()  const override;
    std::string PluginLocal() const override;

    QWidget* CreateGUI(QWidget *Parent) const override;
};
