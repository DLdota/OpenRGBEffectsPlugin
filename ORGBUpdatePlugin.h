#pragma once

#include "ORGBPluginInterface.h"
#include "Dependencies/ResourceManager.h"
#include <QFrame>
#include "ui_ORGBUpdatePlugin.h"


#include <QObject>
#include <QString>
#include <QtPlugin>
#include "QWidget"
#include "QLabel"
#include "QPushButton"
#include "QDialog"
#include "QAction"

class ORGBPlugin : public QObject, public ORGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ORGBPluginInterface_IID)
    Q_INTERFACES(ORGBPluginInterface)

public:
    ~ORGBPlugin() override {}

    bool        HasCustomIcon() const override;
    QLabel*     TabLabel()      const override;

    std::string PluginName() const override;
    std::string PluginDesc()  const override;
    std::string PluginLocal() const override;

    QWidget *CreateGUI(QWidget *Parent, ResourceManager *RM, bool DarkTheme) const override;

    static bool DarkTheme;
    static ResourceManager *RM;
};

class OpenRGBUpdateInfoPage : public QFrame
{
    Q_OBJECT

public:
    OpenRGBUpdateInfoPage(QWidget *parent = nullptr);

private slots:
    void on_CheckButton_clicked();
    void on_UpdateButton_clicked();

private:
    Ui::OpenRGBUpdateInfoPageUi *ui;
    QByteArray GetPageSourceOrFile(QString Link);

    #ifdef _WIN32
    bool WinExtensions = true;
    #else
    bool WinExtensions = false;
    #endif

    /*-------------------------------------------------*\
    | Defaults to primary master if it hasn't been set  |
    \*-------------------------------------------------*/

    QString CheckBranch = "master";
    QString Fork        = "CalcProgrammer1";

    QString CommitLink;
    std::vector<QStringList> Parsed;

    QString PreferedPlatform;
    QString JobID;

    std::vector<QStringList> ParseCommitList(QString ContentToParse);

    bool OwnedByUpdateButton = false;
    bool ContentMissing = false;
    void UpdateProgressBar(bool Show, QString Text = NULL, int Value = NULL);
};
