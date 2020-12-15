#pragma once

#include "ORGBPluginInterface.h"
#include "Dependencies/ResourceManager.h"

#include "ui_PluginPage.h"
#include <QObject>
#include <QString>
#include <QtPlugin>
#include "QWidget"
#include "QLabel"
#include "QPushButton"
#include "QDialog"
#include "QAction"
#include "QTabBar"
#include "QLayout"

class ORGBPlugin : public QObject, public ORGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ORGBPluginInterface_IID)
    Q_INTERFACES(ORGBPluginInterface)

public:
    ~ORGBPlugin() override {}

    std::string PluginName() const override;
    std::string PluginDesc()  const override;
    std::string PluginLocal() const override;

    QWidget* CreateGUI(QWidget *Parent, ResourceManager *RM = nullptr) const override;
private:
    QWidget* CreateInfoTab() const;
};

namespace Ui{
class PluginUi;
}

class Ui::PluginUi : public QFrame
{
    Q_OBJECT

public:
    explicit PluginUi(QWidget *parent = nullptr);
    ~PluginUi();

    Ui::PluginUi *ui;
};
