#pragma once

#include <QtPlugin>
#include <QLabel>
#include "Dependencies/ResourceManager.h"

class QString;
#define ORGBPluginInterface_IID "com.ORGBPluginInterface"

class ORGBPluginInterface
{
public:
    virtual ~ORGBPluginInterface() {}

    virtual bool        HasCustomIcon() = 0;
    virtual QLabel*     TabLabel() = 0;

    virtual std::string PluginName() = 0;
    virtual std::string PluginDesc() = 0;
    virtual std::string PluginLocal() = 0;

    virtual QWidget*    CreateGUI(QWidget *Parent, ResourceManager *RM, bool DarkTheme) = 0;
};

Q_DECLARE_INTERFACE(ORGBPluginInterface, ORGBPluginInterface_IID)
