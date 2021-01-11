#include "OpenRGBPluginInterface.h"
#include "ResourceManager.h"

#include <QObject>
#include <QString>
#include <QtPlugin>
#include "QWidget"
#include "QLabel"
#include "QPushButton"
#include "QDialog"
#include "QAction"

#pragma once

class ORGBPlugin : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID)
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    ~ORGBPlugin() {};

    OpenRGBPluginInfo      PInfo;

    OpenRGBPluginInfo      Initialize(bool dark_theme, ResourceManager* resource_manager_ptr)   override;

    QWidget         *CreateGUI(QWidget *Parent)                                                 override;

    static bool         DarkTheme;

    static ResourceManager* RMPointer;
};
