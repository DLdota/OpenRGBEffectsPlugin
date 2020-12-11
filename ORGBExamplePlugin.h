#pragma once

#include "ORGBExamplePluginInterface.h"

#include <QObject>
#include <QString>
#include <QtPlugin>

class ORGBPlugin : public QObject, public ORGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ORGBPluginInterface_IID)
    Q_INTERFACES(ORGBPluginInterface)

public:
    ~ORGBPlugin() override {}

    std::string PluginName() const override;
};
