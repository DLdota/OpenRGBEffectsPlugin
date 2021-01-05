#ifndef OPENRGBUPDATETAB_H
#define OPENRGBUPDATETAB_H

#include <QFrame>
#include "QWidget"
#include "QLabel"
#include "QDialog"
#include "QAction"
#include "ResourceManager.h"
#include "ui_ORGBUpdatePlugin.h"

#include <QResizeEvent>
#include <QTableView>

#include <QPalette>
#include <QLayout>
#include <QDialog>
#include <QLabel>
#include <QFrame>
#include <QWidget>

#include <QNetworkRequest>
#include <QWebEnginePage>
#include <QSslSocket>
#include <QtNetwork>
#include <QUrl>

#pragma once

class OpenRGBUpdateInfoPage : public QFrame
{
    Q_OBJECT

public:
    OpenRGBUpdateInfoPage(QWidget *Parent = nullptr);
    ~OpenRGBUpdateInfoPage();
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
    QString CheckBranch;
    QString Fork;

    QString CommitLink;
    std::vector<QStringList> Parsed;

    QString PreferedPlatform;
    QString JobID;

    std::vector<QStringList> ParseCommitList(QString ContentToParse);

    bool OwnedByUpdateButton = false;
    bool ContentMissing = false;
    void UpdateProgressBar(bool Show, QString Text = NULL, int Value = NULL);
};


#endif // OPENRGBUPDATETAB_H
