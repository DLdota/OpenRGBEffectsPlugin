#ifndef OPENRGBEFFECTTAB_H
#define OPENRGBEFFECTTAB_H

#include "ui_OpenRGBEffectTab.h"
#include "RGBEffect.h"

namespace Ui {
class OpenRGBEffectTab;
}

class OpenRGBEffectTab : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBEffectTab(QWidget *parent = nullptr);
    ~OpenRGBEffectTab();

public slots:
    void DeviceListChanged();

private slots:
    void on_device_list_SelectionChanged();
    void on_EffectTabs_currentChanged(int);
    void on_save_settings_clicked();
    void on_plugin_infos_clicked();

private:
    Ui::OpenRGBEffectTab                *ui;

    void InitEffectTabs();
    void CreateEffectTab(RGBEffect*);
    void InitDeviceList();    

    void LoadEffectsFromSettings();
    void LoadEffectSettings(json);

    void SetFirstTabStyle();
};

#endif // OPENRGBEFFECTTAB_H
