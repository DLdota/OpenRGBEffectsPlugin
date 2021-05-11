#ifndef OPENRGBEFFECTSPLUGIN_H
#define OPENRGBEFFECTSPLUGIN_H

#include <QObject>
#include <QWidget>

#include "OpenRGBPluginInterface.h"
#include "ResourceManager.h"

class OpenRGBEffectsPlugin : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID)
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    ~OpenRGBEffectsPlugin() {};

    OpenRGBPluginInfo      PInfo;

    OpenRGBPluginInfo      Initialize(bool dark_theme, ResourceManager* resource_manager_ptr)   override;

    QWidget                *CreateGUI(QWidget *Parent)                                          override;

    inline static bool DarkTheme = false;
    inline static ResourceManager* RMPointer = nullptr;
private:
    static void DeviceListChangedCallback(void* ptr);
};

#endif
