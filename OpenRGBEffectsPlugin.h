#ifndef OPENRGBEFFECTSPLUGIN_H
#define OPENRGBEFFECTSPLUGIN_H

#include <QObject>
#include <QWidget>

#include "OpenRGBPluginInterface.h"
#include "OpenRGBEffectTab.h"
#include "ResourceManager.h"

class OpenRGBEffectsPlugin : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID)
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    ~OpenRGBEffectsPlugin() {};

    /*-------------------------------------------------------------------------------------------------*\
    | Plugin Information                                                                                |
    \*-------------------------------------------------------------------------------------------------*/
    virtual OpenRGBPluginInfo   GetPluginInfo()                                                     override;
    virtual unsigned int        GetPluginAPIVersion()                                               override;

    /*-------------------------------------------------------------------------------------------------*\
    | Plugin Functionality                                                                              |
    \*-------------------------------------------------------------------------------------------------*/
    virtual void                Load(bool dark_theme, ResourceManager* resource_manager_ptr)        override;
    virtual QWidget*            GetWidget()                                                         override;
    virtual QMenu*              GetTrayMenu()                                                       override;
    virtual void                Unload()                                                            override;

    /*-------------------------------------------------------------------------------------------------*\
    | Plugin Variables                                                                                  |
    \*-------------------------------------------------------------------------------------------------*/
    static bool                 DarkTheme;
    static ResourceManager*     RMPointer;

private:
    static void                 DeviceListChangedCallback(void* ptr);
    OpenRGBEffectTab*           ui;
};

#endif
