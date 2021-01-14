#ifndef OPENRGBEFFECTTAB_H
#define OPENRGBEFFECTTAB_H

#include <QWidget>
#include "ui_OpenRGBEffectTab.h"
#include "ORGBEffectPlugin.h"

#include "RGBEffect.h"
#include "RGBController.h"

#include "SpectrumCycling.h"
#include "RainbowWave.h"

#pragma once

struct EffectInstStruct
{
    RGBEffect* EffectInst;
    std::vector<RGBController*> OwnedControllers;
};

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

    void CreateDeviceSelection(std::string DeviceName);
private slots:
    void DeviceListChanged();
    void DeviceSelectionChanged();

    void on_TabChange(int Tab);
private:
    Ui::OpenRGBEffectTab                *ui;
    int                                 CurrentTab;
    /*
    | List of static items (Possible effects and controllers)
    */
    static std::vector<EffectInstStruct>        EffectList;
    static std::vector<BetterController>        Controllers;

    /*-----------------------------------------*\
    | Effect Handling (Stepping and active)     |
    \*-----------------------------------------*/
    static std::vector<RGBEffect*>      ActiveEffects;
    std::thread*                        StepEffectThread;
    void                                EffectStepTimer();

    static void DeviceListChangedCallback(void* ptr);
};

#endif // OPENRGBEFFECTTAB_H
