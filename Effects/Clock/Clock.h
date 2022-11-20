#ifndef CLOCK_H
#define CLOCK_H

#include <QWidget>
#include "ui_Clock.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class Clock;
}

class Clock : public RGBEffect
{
    Q_OBJECT

public:
    explicit Clock(QWidget *parent = nullptr);
    ~Clock();

    EFFECT_REGISTERER(ClassName(), CAT_SIMPLE, [](){return new Clock;});
    static std::string const ClassName() {return "Clock";}
    void StepEffect(std::vector<ControllerZone*>) override;

private:
    Ui::Clock *ui;

    float h = 0.f;
    float m = 0.f;
    float s = 0.f;

    RGBColor GetColor(float, float);
};

#endif // CLOCK_H
