#ifndef OPENRGBEFFECTTAB_H
#define OPENRGBEFFECTTAB_H

#include <QWidget>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSignalMapper>

#include "ui_OpenRGBEffectTab.h"
#include "ORGBEffectPlugin.h"

#include "RGBEffect.h"
#include "RGBController.h"

#include "SpectrumCycling.h"
#include "RainbowWave.h"

#pragma once

namespace Ui {
class OpenRGBEffectTab;
}

class OpenRGBEffectTab : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBEffectTab(QWidget *parent = nullptr);
    ~OpenRGBEffectTab();

    void DefineEffects();

    static void SetEffectActive(RGBEffect* Effect);
    static void SetEffectUnActive(RGBEffect* Effect);

    void CreateDeviceSelection(RGBController* Controller, int Index, bool HasDirectMode);

    static bool CheckReversed (int DeviceIndex, int ZoneIndex);
private slots:
    void DeviceListChanged();

    void DeviceSelectionChanged(QString DName);
    void ZoneSelectionChanged(QString DName);

    void DeviceReversalChanged(QString DName);
    void ZoneReversalChanged(QString DName);

    void on_TabChange(int Tab);

    void FPSSlider(int SpeedIndex);
private:
    Ui::OpenRGBEffectTab                *ui;
    int                                 CurrentTab;
    /*--------------------------------------------------------*\
    | List of static items (Possible effects and controllers)  |
    \*--------------------------------------------------------*/
    static std::vector<RGBEffect*>                              EffectList;
    static std::vector<std::vector<OwnedControllerAndZones>>    RespectiveToPass;

    static std::vector<BetterController>                        Controllers;

    /*------------------*\
    | FPS list/current   |
    \*------------------*/
    int                                 FPSDelay;
    int                                 FPS;

    static std::vector<int>             GetSpeed;

    /*-----------------------------------------*\
    | Effect Handling (Stepping and active)     |
    \*-----------------------------------------*/
    static std::vector<RGBEffect*>      ActiveEffects;
    std::thread*                        StepEffectThread;
    void                                EffectStepTimer();

    static void DeviceListChangedCallback(void* ptr);
};

#endif // OPENRGBEFFECTTAB_H
