#ifndef OPENRGBEFFECTPAGE_H
#define OPENRGBEFFECTPAGE_H

#include "ui_OpenRGBEffectPage.h"

#include "RGBEffect.h"
#include "ColorPicker.h"

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
    void DeviceListChanged(std::vector<BetterController> Devices);

private slots:
    void on_StartButton_clicked();
    void on_StopButton_clicked();
    void on_SpeedSlider_valueChanged(int value);
    void on_Slider2_valueChanged(int value);
    void on_SaveSettings_clicked();
    void on_AutoStart_clicked();
    void on_RandomCheckbox_clicked();
    void on_OnlyFirst_clicked();

private:
    Ui::OpenRGBEffectPage *ui;

    RGBEffect* EFCT;

    std::vector<RGBController*> OwnedController;
    std::vector<RGBColor> UserColors;
    std::vector<ColorPicker*> ColorPickers;

    bool AutoStart = false;
    bool RandomColors = false;

    void InitUi();
    void LoadStartupSettings();
    void StartEffect();
    void StopEffect();
};

#endif // OPENRGBEFFECTPAGE_H
