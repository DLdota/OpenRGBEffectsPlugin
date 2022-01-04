#ifndef ROTATINGRAINBOW_H
#define ROTATINGRAINBOW_H

#include <QWidget>
#include "ui_RotatingRainbow.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"


namespace Ui {
class RotatingRainbow;
}

class RotatingRainbow : public RGBEffect
{
    Q_OBJECT

public:
    explicit RotatingRainbow(QWidget *parent = nullptr);
    ~RotatingRainbow();

    EFFECT_REGISTERER(ClassName(), CAT_RAINBOW, [](){return new RotatingRainbow;});

    static std::string const ClassName() {return "RotatingRainbow";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;

private:
    Ui::RotatingRainbow *ui;

    double time = 1000.0;

    double cos_time;
    double sin_time;

    double cos_reverse_time;
    double sin_reverse_time;

    double color_speed = 1.f;

    RGBColor GetColor(float, float, float, float, bool);
};

#endif // ROTATINGRAINBOW_H
