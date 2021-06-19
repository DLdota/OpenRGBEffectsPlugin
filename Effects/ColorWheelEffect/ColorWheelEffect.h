#ifndef COLORWHEELEFFECT_H
#define COLORWHEELEFFECT_H

#include <QWidget>
#include "ui_ColorWheelEffect.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class ColorWheelEffect;
}

class ColorWheelEffect : public QWidget, public RGBEffect
{
    Q_OBJECT

public:
    explicit ColorWheelEffect(QWidget *parent = nullptr);
    ~ColorWheelEffect();

    EFFECT_REGISTERER(ClassName(), [](){return new ColorWheelEffect;});

    static std::string const ClassName() {return "ColorWheel";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone>) override;

private:
    Ui::ColorWheelEffect *ui;
    double progress = 0.f;
    RGBColor GetColor(unsigned int, unsigned int, double, double, bool);
};

#endif // COLORWHEELEFFECT_H
