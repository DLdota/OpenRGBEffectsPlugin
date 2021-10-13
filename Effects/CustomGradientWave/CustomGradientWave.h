#ifndef CUSTOMGRADIENTWAVE_H
#define CUSTOMGRADIENTWAVE_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include <QWidget>
#include "ColorPicker.h"
#include "ui_CustomGradientWave.h"

namespace Ui {
class CustomGradientWave;
}

class CustomGradientWave : public RGBEffect
{
    Q_OBJECT

public:
    explicit CustomGradientWave(QWidget *parent = nullptr);
    ~CustomGradientWave();

    EFFECT_REGISTERER(ClassName(), [](){return new CustomGradientWave;});

    static std::string const ClassName() {return "CustomGradientWave";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_colors_count_spinBox_valueChanged(int);

private:
    Ui::CustomGradientWave *ui;
    double progress = 0.0;
    RGBColor GetColor(float, float);
    void ResetColors();
    ColorPicker* CreatePicker(int);

    std::vector<RGBColor> colors;
    std::vector<ColorPicker*> color_pickers;
    QImage gradient;
    void GenerateGradient();
};

#endif // CUSTOMGRADIENTWAVE_H
