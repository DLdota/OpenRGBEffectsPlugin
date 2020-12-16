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

    virtual bool        HasCustomIcon()         const = 0;
    virtual QLabel*     TabLabel()              const = 0;

    virtual std::string PluginName()            const = 0;
    virtual std::string PluginDesc()            const = 0;
    virtual std::string PluginLocal()           const = 0;

    virtual QWidget* CreateGUI(QWidget *Parent) const = 0;
    virtual void SetRM(ResourceManager *RM)     const = 0;
};

Q_DECLARE_INTERFACE(ORGBPluginInterface, ORGBPluginInterface_IID)
