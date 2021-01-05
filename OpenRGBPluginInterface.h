#pragma once

#include <QtPlugin>
#include <QLabel>
#include "OpenRGB/ResourceManager.h"

#define OpenRGBPluginInterface_IID "com.OpenRGBPluginInterface"

struct OpenRGBPluginInfo
{
    std::string                 PluginName;
    std::string                 PluginDesc;
    std::string                 PluginLoca;

    bool                        HasCustom;
    QLabel                      *PluginLabel;
};

class OpenRGBPluginInterface
{
public:
    virtual                 ~OpenRGBPluginInterface() {}

    OpenRGBPluginInfo              PInfo;

    virtual OpenRGBPluginInfo      init(bool DarkTheme, ResourceManagerInterface *RM) = 0;

    virtual QWidget         *CreateGUI(QWidget *Parent) = 0;
};

Q_DECLARE_INTERFACE(OpenRGBPluginInterface, OpenRGBPluginInterface_IID)
