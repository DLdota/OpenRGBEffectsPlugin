#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include "json.hpp"
#include "ui_GlobalSettings.h"

#include <QWidget>

namespace Ui {
class GlobalSettings;
}

class GlobalSettings : public QWidget
{
    Q_OBJECT

public:
    explicit GlobalSettings(QWidget *parent = nullptr);
    ~GlobalSettings();

private slots:
    void on_fpscaptureSlider_valueChanged(int);
    void on_brightnessSlider_valueChanged(int);
    void on_fpsSlider_valueChanged(int);
    void on_hide_unsupportedCheckBox_stateChanged(int);
    void on_usePreferedColorsCheckBox_stateChanged(int);
    void on_preferedColors_ColorsChanged();
    void on_randomColorsCheckBox_stateChanged(int);

private:
    Ui::GlobalSettings *ui;

};

#endif // GLOBALSETTINGS_H
