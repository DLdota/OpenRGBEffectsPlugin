#include "ORGBPluginInterface.h"
#include "Dependencies/ResourceManager.h"
#include "OpenRGBUpdateTab.h"

#include <QObject>
#include <QString>
#include <QtPlugin>
#include "QWidget"
#include "QLabel"
#include "QPushButton"
#include "QDialog"
#include "QAction"
#include "Dependencies/json.hpp"

class ORGBPlugin : public QObject, public ORGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ORGBPluginInterface_IID)
    Q_INTERFACES(ORGBPluginInterface)

public:
                    ~ORGBPlugin() override {}

    PluginInfo      DefineNeeded() override;

    PluginInfo      PInfo;

    PluginInfo      init(json Settings , bool DarkTheme) override;

    QWidget         *CreateGUI(QWidget *Parent) override;

    static bool     DarkTheme;
    static std::string branch;
    static std::string fork;
};
