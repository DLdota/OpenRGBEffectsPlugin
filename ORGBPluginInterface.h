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

#define ORGBPluginInterface_IID "com.ORGBPluginInterface"

struct PluginInfo
{
    std::string                 PluginName;
    std::string                 PluginDesc;
    std::string                 PluginLoca;

    bool                        HasCustom;
    QLabel                      *PluginLabel;

    std::string                 SettingName;
};

class ORGBPluginInterface
{
public:
    virtual                 ~ORGBPluginInterface() {}

    PluginInfo              PInfo;

    virtual PluginInfo      DefineNeeded() = 0;

    virtual PluginInfo      init(json Settings , bool DarkTheme) = 0;

    virtual QWidget         *CreateGUI(QWidget *Parent) = 0;
};

Q_DECLARE_INTERFACE(ORGBPluginInterface, ORGBPluginInterface_IID)
