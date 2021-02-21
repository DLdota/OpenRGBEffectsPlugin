#ifndef OPENRGBEFFECTPAGE_H
#define OPENRGBEFFECTPAGE_H

#include <QFile>
#include <QWidget>
#include <QCheckBox>
#include <QColorDialog>
#include <QApplication>

#include <fstream>
#include <iostream>

#include <thread>

#include "ORGBEffectPlugin.h"
#include "OpenRGBEffectTab.h"
#include "ui_OpenRGBEffectPage.h"
#include "OpenRGBEffectSettings.h"

struct EffectSettings
{
    std::vector<RGBColor> UserColors;
    int                   SpeedSliderVal;
    int                   Slider2Val;
};

namespace Ui {
class OpenRGBEffectPage;
}

class OpenRGBEffectPage : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBEffectPage(QWidget *parent = nullptr, RGBEffect* EFCT = nullptr);
    ~OpenRGBEffectPage();

    void CreateDeviceSelection(std::string DeviceName);

    bool HasSpeed = false;
    bool RandomColors = false;

    void DeviceListChanged(std::vector<BetterController> Devices);

private slots:
    void on_StartButton_clicked();
    void on_StopButton_clicked();
    void on_SpeedSlider_valueChanged(int value);
    void on_Slider2_valueChanged(int value);
    void on_UserColorNum_currentIndexChanged(int NewIndex);
    void on_ColorPickerButton_clicked();
    void on_SaveSettings_clicked();
    void on_AutoStart_clicked();
    void on_RandomCheckbox_clicked();


private:
    Ui::OpenRGBEffectPage *ui;
    RGBEffect* EFCT;

    std::vector<RGBController*> OwnedController;

    std::vector<RGBColor> UserColors;
    int                   CurrentColor = 0;

    bool AutoStart = false;
    void StartupSettings();
};

#endif // OPENRGBEFFECTPAGE_H
