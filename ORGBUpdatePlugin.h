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

    void DefineNeeded() override;

    PluginDataNeeded    NeededInfo;

    bool                HasCustomIcon() override;
    QLabel*             TabLabel() override;

    std::string         PluginName() override;
    std::string         PluginDesc() override;
    std::string         PluginLocal() override;

    QWidget *CreateGUI(QWidget *Parent) override;

    static bool DarkTheme;
};
