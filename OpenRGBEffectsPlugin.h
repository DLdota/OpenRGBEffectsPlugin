#ifndef OPENRGBEFFECTSPLUGIN_H
#define OPENRGBEFFECTSPLUGIN_H

#include <QObject>
#include <QWidget>

#include "OpenRGBPluginInterface.h"
#include "OpenRGBEffectTab.h"
#include "ResourceManager.h"
#include "NetworkServer.h"

enum {
    NET_PACKET_ID_REQUEST_EFFECT_LIST   = 0,
    NET_PACKET_ID_START_EFFECT          = 20,
    NET_PACKET_ID_STOP_EFFECT           = 21
};

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
    virtual OpenRGBPluginInfo   GetPluginInfo()                                                                 override;
    virtual unsigned int        GetPluginAPIVersion()                                                           override;

    /*-------------------------------------------------------------------------------------------------*\
    | Plugin Functionality                                                                              |
    \*-------------------------------------------------------------------------------------------------*/
    virtual void                Load(bool dark_theme, ResourceManager* resource_manager_ptr)                    override;
    virtual QWidget*            GetWidget()                                                                     override;
    virtual QMenu*              GetTrayMenu()                                                                   override;
    virtual void                Unload()                                                                        override;
    static unsigned char*       HandleSDK(void * instance, unsigned int pkt_id, unsigned char* data, unsigned int* data_size);

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
