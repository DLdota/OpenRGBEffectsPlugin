#ifndef OPENRGBEFFECTPAGE_H
#define OPENRGBEFFECTPAGE_H

#include <QWidget>
#include <QCheckBox>

namespace Ui {
class OpenRGBEffectPage;
}

class OpenRGBEffectPage : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBEffectPage(QWidget *parent = nullptr);
    ~OpenRGBEffectPage();

    void CreateDeviceSelection(std::string DeviceName);

private:
    Ui::OpenRGBEffectPage *ui;
};

#endif // OPENRGBEFFECTPAGE_H
