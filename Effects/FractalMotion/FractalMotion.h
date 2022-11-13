#ifndef FRACTALMOTION_H
#define FRACTALMOTION_H

#include <QWidget>
#include "RGBEffect.h"
#include "ui_FractalMotion.h"
#include "EffectRegisterer.h"
#include "ColorUtils.h"

namespace Ui {
class FractalMotion;
}

class FractalMotion : public RGBEffect
{
    Q_OBJECT

public:
    explicit FractalMotion(QWidget *parent = nullptr);
    ~FractalMotion();

    EFFECT_REGISTERER(ClassName(), CAT_ADVANCED, [](){return new FractalMotion;});

    static std::string const ClassName() {return "FractalMotion";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private slots:
    void on_background_ColorSelected(QColor);

    void on_thickness_valueChanged(int);
    void on_amplitude_valueChanged(int);
    void on_frequency_valueChanged(int);

    void on_freq_m1_valueChanged(int);
    void on_freq_m2_valueChanged(int);
    void on_freq_m3_valueChanged(int);
    void on_freq_m4_valueChanged(int);
    void on_freq_m5_valueChanged(int);
    void on_freq_m6_valueChanged(int);
    void on_freq_m7_valueChanged(int);
    void on_freq_m8_valueChanged(int);
    void on_freq_m9_valueChanged(int);
    void on_freq_m10_valueChanged(int);
    void on_freq_m11_valueChanged(int);
    void on_freq_m12_valueChanged(int);
    void on_defaults_clicked();

private:
    Ui::FractalMotion *ui;
    RGBColor GetColor(unsigned int, unsigned int, unsigned int);

    double progress = 0.f;
    RGBColor background = ColorUtils::OFF();

    void Defaults();

    unsigned int default_thickness = 2;
    float default_amplitude = 1.f;
    float default_frequency = 1.f;
    float default_freq_m1 = 2.1;
    float default_freq_m2 = 4.5;
    float default_freq_m3 = 1.72;
    float default_freq_m4 = 1.121;
    float default_freq_m5 = 4.0;
    float default_freq_m6 = 2.221;
    float default_freq_m7 = 0.437;
    float default_freq_m8 = 5.0;
    float default_freq_m9 = 2.1122;
    float default_freq_m10 = 1.5;
    float default_freq_m11 = 1.72;
    float default_freq_m12 = 0.06;

    unsigned int thickness = default_thickness;
    float amplitude = default_amplitude;
    float frequency = default_frequency;
    float freq_m1 = default_freq_m1;
    float freq_m2 = default_freq_m2;
    float freq_m3 = default_freq_m3;
    float freq_m4 = default_freq_m4;
    float freq_m5 = default_freq_m5;
    float freq_m6 = default_freq_m6;
    float freq_m7 = default_freq_m7;
    float freq_m8 = default_freq_m8;
    float freq_m9 = default_freq_m9;
    float freq_m10 = default_freq_m10;
    float freq_m11 = default_freq_m11;
    float freq_m12 = default_freq_m12;

    unsigned int val_mult = 100;
    float inv_val_mult = 1.f / val_mult;

    float random_tick = 0.f;
    RGBColor random_color;
    RGBColor next_random_color;

};

#endif // FRACTALMOTION_H
