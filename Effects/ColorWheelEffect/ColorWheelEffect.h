#ifndef COLORWHEELEFFECT_H
#define COLORWHEELEFFECT_H

#include <QWidget>
#include "ui_ColorWheelEffect.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class ColorWheelEffect;
}

class ColorWheelEffect : public RGBEffect
{
    Q_OBJECT

public:
    explicit ColorWheelEffect(QWidget *parent = nullptr);
    ~ColorWheelEffect();

    EFFECT_REGISTERER(ClassName(), CAT_BASIC, [](){return new ColorWheelEffect;});

    static std::string const ClassName() {return "ColorWheel";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_cx_valueChanged(int);
    void on_cy_valueChanged(int);
    void on_direction_currentIndexChanged(int);

private:
    Ui::ColorWheelEffect *ui;
    double progress = 0.f;
    RGBColor GetColor(unsigned int, unsigned int, double, double, bool);

    unsigned int cx_shift = 50;
    unsigned int cy_shift = 50;
    unsigned int direction = 0;
};

#endif // COLORWHEELEFFECT_H
