#ifndef OPENRGBEFFECTTAB_H
#define OPENRGBEFFECTTAB_H

#include <QCheckBox>

#include "ui_OpenRGBEffectTab.h"

#include "ResetButton.h"
#include "RGBController.h"

namespace Ui {
class OpenRGBEffectTab;
}

class OpenRGBEffectTab : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBEffectTab(QWidget *parent = nullptr);
    ~OpenRGBEffectTab();

private slots:
    void DeviceListChanged();

    void DeviceSelectionChanged(QString DName);
    void ZoneSelectionChanged(QString DName);

    void DeviceReversalChanged(QString DName);
    void ZoneReversalChanged(QString DName);

    void on_LeftTabBar_currentChanged(int);
    void on_FPSSlider_valueChanged(int);
    void on_SelectAll_clicked();

    void UnlockDevice(int Device, int Zone);

private:
    Ui::OpenRGBEffectTab                *ui;
    int                                 CurrentTab = 0;

    static void DeviceListChangedCallback(void* ptr);
    void CreateDeviceSelection(RGBController* Controller, int Index, bool HasDirectMode);
    void SetStyleSheetMargins(QCheckBox* CB);

    QCheckBox* GetCheckboxFromFrame(QWidget*);
    ResetButton* GetResetButtonFromFrame(QWidget*);

    bool                                SelectsAll    = true;
    const std::vector<int>              Speeds;

    void SaveReversedSettings();
};

#endif // OPENRGBEFFECTTAB_H
