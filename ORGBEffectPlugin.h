#ifndef ORGBEFFECTPLUGIN_H
#define ORGBEFFECTPLUGIN_H

#include <QObject>
#include <QString>
#include <QtPlugin>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QAction>

#include "OpenRGBPluginInterface.h"
#include "ResourceManager.h"

class ORGBPlugin : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID)
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    ~ORGBPlugin() {};

    OpenRGBPluginInfo      PInfo;

    OpenRGBPluginInfo      Initialize(bool dark_theme, ResourceManager* resource_manager_ptr)   override;

    QWidget                *CreateGUI(QWidget *Parent)                                          override;

    inline static bool             DarkTheme = false;
    inline static ResourceManager* RMPointer = nullptr;
};

#endif
