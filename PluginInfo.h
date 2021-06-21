#ifndef PLUGININFO_H
#define PLUGININFO_H

#include <QWidget>
#include "ui_PluginInfo.h"

namespace Ui {
class PluginInfo;
}

class PluginInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PluginInfo(QWidget *parent = nullptr);
    ~PluginInfo();

private slots:
    void on_open_plugin_folder_clicked();
    void on_download_latest_clicked();

private:
    Ui::PluginInfo *ui;
};

#endif // PLUGININFO_H
