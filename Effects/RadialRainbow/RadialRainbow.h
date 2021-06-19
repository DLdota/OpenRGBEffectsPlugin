#ifndef RADIALRAINBOW_H
#define RADIALRAINBOW_H

#include <QWidget>
#include "ui_RadialRainbow.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class RadialRainbow;
}

class RadialRainbow : public QWidget, public RGBEffect
{
    Q_OBJECT

public:
    explicit RadialRainbow(QWidget *parent = nullptr);
    ~RadialRainbow();

    EFFECT_REGISTERER(ClassName(), [](){return new RadialRainbow;});

    static std::string const ClassName() {return "RadialRainbow";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone>) override;

private:
    Ui::RadialRainbow *ui;
    double progress = 0.f;
    RGBColor GetColor(unsigned int, unsigned int, double, double, bool);
};

#endif // RADIALRAINBOW_H
