#ifndef OPENRGBEFFECTPAGE_H
#define OPENRGBEFFECTPAGE_H

#include <QWidget>
#include <QCheckBox>
#include <QColorDialog>

#include <thread>

#include "OpenRGBEffectTab.h"
#include "ui_OpenRGBEffectPage.h"

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

    void DeviceListChanged(std::vector<BetterController> Devices);

private slots:
    void on_StartButton_clicked();
    void on_StopButton_clicked();
    void on_SpeedSlider_valueChanged(int value);
    void on_Slider2_valueChanged(int value);
    void on_toolButton_clicked();


private:
    Ui::OpenRGBEffectPage *ui;
    RGBEffect* EFCT;

    std::vector<RGBController*> OwnedController;

    std::vector<RGBColor> UserColors;
};

#endif // OPENRGBEFFECTPAGE_H
