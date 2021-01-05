#include "OpenRGBPluginInterface.h"
#include "ResourceManager.h"
#include "OpenRGBUpdateTab.h"

#include <QObject>
#include <QString>
#include <QtPlugin>
#include "QWidget"
#include "QLabel"
#include "QPushButton"
#include "QDialog"
#include "QAction"

class ORGBPlugin : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID)
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    ~ORGBPlugin()                                                               override {}

    OpenRGBPluginInfo      DefineNeeded()                                       override;

    OpenRGBPluginInfo      PInfo;

    OpenRGBPluginInfo      init(bool DarkTheme, ResourceManagerInterface *RM)   override;

    QWidget         *CreateGUI(QWidget *Parent)                                 override;

    static bool         DarkTheme;
    static std::string  branch;
    static std::string  fork;
};
