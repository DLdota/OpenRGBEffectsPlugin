#ifndef WAVY_H
#define WAVY_H


#include "ui_Wavy.h"
#include <QWidget>
#include "RGBEffect.h"
#include "EffectRegisterer.h"

#include <math.h>
#include <algorithm>

namespace Ui {
class Wavy;
}

class Wavy: public RGBEffect
{
    Q_OBJECT

public:
    explicit Wavy(QWidget *parent = nullptr);
    ~Wavy();

    EFFECT_REGISTERER(ClassName(), CAT_ADVANCED, [](){return new Wavy;});

    static std::string const ClassName() {return "Wavy";}

    void DefineExtraOptions(QLayout*)  override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private:
    Ui::Wavy   *ui;
    bool       Dir = true ;
    float      SineProgress      = 0.0f;
    float      WaveProgress      = 0.0f;

    float      WaveFrequency     = 1;
    float      WaveSpeed         = 1;
    float      OscillationSpeed  = 1;

    std::vector<RGBColor>   RandomColors;

    const double PI = std::atan(1)*4;

    RGBColor GetColor(int, int);
    void GenerateRandomColors();

private slots:
    void on_wave_freq_slider_valueChanged(int);
    void on_wave_speed_slider_valueChanged(int);
    void on_oscillation_speed_slider_valueChanged(int);
};

#endif // WAVY_H
