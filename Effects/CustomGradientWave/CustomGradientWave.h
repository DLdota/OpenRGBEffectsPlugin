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

    EFFECT_REGISTERER(ClassName(), CAT_RAINBOW, [](){return new CustomGradientWave;});

    static std::string const ClassName() {return "CustomGradientWave";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_colors_count_spinBox_valueChanged(int);
    void on_preset_currentTextChanged(const QString&);

private:
    Ui::CustomGradientWave *ui;
    double progress = 0.0;
    RGBColor GetColor(float, float);
    void ResetColors();
    ColorPicker* CreatePicker(int);
    void LoadPreset(const QString&);

    std::vector<RGBColor> colors;
    std::vector<ColorPicker*> color_pickers;
    QImage gradient;
    void GenerateGradient();

    std::map<std::string,std::vector<RGBColor>> presets = {
        {"Default", std::vector<RGBColor>{
            262399,
            15073535,
            16711680,
            16757504,
            5373696,
            65514,
            46079,
            255
         }},

        {"Pink/Blue", std::vector<RGBColor>{
             16711680,
             16711935,
             16711680
         }},

        {"Pink/Gold", std::vector<RGBColor>{
             57855,
             16711935,
             57855
         }}
    };
};

#endif // CUSTOMGRADIENTWAVE_H
