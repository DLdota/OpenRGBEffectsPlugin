#ifndef OPENRGBEFFECTTAB_H
#define OPENRGBEFFECTTAB_H

#include "ui_OpenRGBEffectTab.h"
#include "RGBEffect.h"
#include "EffectList.h"

namespace Ui {
class OpenRGBEffectTab;
}

class OpenRGBEffectTab : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBEffectTab(QWidget *parent = nullptr);
    ~OpenRGBEffectTab();

    void StartAll();
    void StopAll();

    void LoadProfile(std::string);

signals:
    void ProfileListUpdated();

public slots:
    void DeviceListChanged();
    void LoadProfileList();

private slots:
    void on_device_list_SelectionChanged();
    void on_EffectTabs_currentChanged(int);
    void on_save_settings_clicked();
    void on_delete_profile_clicked();
    void on_plugin_infos_clicked();
    void on_profiles_currentIndexChanged(int);

    void OnStopEffects();

private:
    Ui::OpenRGBEffectTab *ui;
    EffectList* effect_list = nullptr;

    void InitEffectTabs();
    void CreateEffectTab(RGBEffect*);
    void InitDeviceList();    

    void LoadEffectsFromCurrentProfile();
    void LoadEffect(json);

    void SetFirstTabStyle();

    void ClearAll();
};

#endif // OPENRGBEFFECTTAB_H
