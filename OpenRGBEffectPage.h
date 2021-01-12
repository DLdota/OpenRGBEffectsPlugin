#ifndef OPENRGBEFFECTPAGE_H
#define OPENRGBEFFECTPAGE_H

#include <QWidget>
#include <QCheckBox>

#include <thread>
#include <QThread>

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

private slots:
    void on_StartButton_clicked();
    void on_StopButton_clicked();
    void on_SpeedSlider_valueChanged(int value);

    void        DeviceListChanged();
private:
    Ui::OpenRGBEffectPage *ui;
    RGBEffect* EFCT;

    std::vector<RGBController*> OwnedController;

    static void DeviceListChangedCallback(void* ptr);
};

#endif // OPENRGBEFFECTPAGE_H
