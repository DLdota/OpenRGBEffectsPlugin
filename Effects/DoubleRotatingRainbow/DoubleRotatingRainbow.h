#ifndef DOUBLEROTATINGRAINBOW_H
#define DOUBLEROTATINGRAINBOW_H

#include <QWidget>
#include "ui_DoubleRotatingRainbow.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"


namespace Ui {
class DoubleRotatingRainbow;
}

class DoubleRotatingRainbow : public RGBEffect
{
    Q_OBJECT

public:
    explicit DoubleRotatingRainbow(QWidget *parent = nullptr);
    ~DoubleRotatingRainbow();

    EFFECT_REGISTERER(ClassName(), [](){return new DoubleRotatingRainbow;});

    static std::string const ClassName() {return "DoubleRotatingRainbow";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_frequency_valueChanged(int);

private:
    Ui::DoubleRotatingRainbow *ui;

    double time = 0.0;

    double cos_time;
    double sin_time;

    double cos_reverse_time;
    double sin_reverse_time;

    double color_speed = 1.f;

    int frequency = 1;

    RGBColor GetColor(float, float, float, float, bool);
};

#endif // DOUBLEROTATINGRAINBOW_H
