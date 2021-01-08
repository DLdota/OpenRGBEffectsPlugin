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

private:
    Ui::OpenRGBEffectPage *ui;
};

#endif // OPENRGBEFFECTPAGE_H
