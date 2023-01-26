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

    void SetEffectState(std::string name, bool running);
    void LoadProfile(std::string);
    unsigned char * GetEffectListDescription(unsigned int* data_size);
signals:
    void ProfileListUpdated();

public slots:
    void DeviceListChanged();
    void LoadProfileList();
    void LoadProfile(QString);
    void StartAll();
    void StopAll();

private slots:
    void on_device_list_SelectionChanged();
    void on_EffectTabs_currentChanged(int);    

    void OnStopEffects();
    void SaveProfileAction();
    void DeleteProfileAction();
    void PluginInfoAction();

private:
    Ui::OpenRGBEffectTab *ui;
    EffectList* effect_list = nullptr;
    QMenu* load_profile_menu;

    std::string latest_loaded_profile = "";

    void InitEffectTabs();
    void CreateEffectTab(RGBEffect*);
    void InitDeviceList();
    void LoadEffect(json);
    void ClearAll();
};

#endif // OPENRGBEFFECTTAB_H
