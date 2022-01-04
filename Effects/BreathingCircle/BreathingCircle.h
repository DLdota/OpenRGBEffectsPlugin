#ifndef BREATHINGCIRCLE_H
#define BREATHINGCIRCLE_H

#include <QWidget>
#include "ui_BreathingCircle.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"

namespace Ui {
class BreathingCircle;
}

class BreathingCircle : public RGBEffect
{
    Q_OBJECT

public:
    explicit BreathingCircle(QWidget *parent = nullptr);
    ~BreathingCircle();

    EFFECT_REGISTERER(ClassName(), CAT_BASIC, [](){return new BreathingCircle;});

    static std::string const ClassName() {return "BreathingCircle";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;

private:
    Ui::BreathingCircle *ui;

    double time = 0.0;
    double progress = 0.0;
    RGBColor GetColor(float, float, float, float);
};

#endif // BREATHINGCIRCLE_H
