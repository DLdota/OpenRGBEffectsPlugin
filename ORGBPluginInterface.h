#pragma once

#include <QtPlugin>
#include <QLabel>
#include "Dependencies/ResourceManager.h"

struct PluginDataNeeded {
    std::vector<int>                        Needed;

    bool                                    DarkTheme;
    std::vector<std::vector<std::string>>   Settings;
    std::vector<RGBController *>            Devices;
    std::vector<NetworkClient *>            Clients;
};


class QString;
#define ORGBPluginInterface_IID "com.ORGBPluginInterface"

class ORGBPluginInterface
{
public:
    virtual ~ORGBPluginInterface() {}

    virtual void        DefineNeeded() = 0;
    PluginDataNeeded    NeededInfo;

    virtual bool        HasCustomIcon() = 0;
    virtual QLabel*     TabLabel() = 0;

    virtual std::string PluginName() = 0;
    virtual std::string PluginDesc() = 0;
    virtual std::string PluginLocal() = 0;

    virtual QWidget*    CreateGUI(QWidget *Parent) = 0;
};

Q_DECLARE_INTERFACE(ORGBPluginInterface, ORGBPluginInterface_IID)
