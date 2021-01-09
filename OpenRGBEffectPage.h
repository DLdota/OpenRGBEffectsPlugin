#ifndef OPENRGBEFFECTPAGE_H
#define OPENRGBEFFECTPAGE_H

#include <QWidget>
#include <QCheckBox>

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
private:
    Ui::OpenRGBEffectPage *ui;
    RGBEffect* EFCT;

    void on_ValueChanged();
};

#endif // OPENRGBEFFECTPAGE_H
