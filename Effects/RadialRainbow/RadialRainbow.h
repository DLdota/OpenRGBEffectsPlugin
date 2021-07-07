#ifndef RADIALRAINBOW_H
#define RADIALRAINBOW_H

#include <QWidget>
#include "ui_RadialRainbow.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class RadialRainbow;
}

class RadialRainbow : public RGBEffect
{
    Q_OBJECT

public:
    explicit RadialRainbow(QWidget *parent = nullptr);
    ~RadialRainbow();

    EFFECT_REGISTERER(ClassName(), [](){return new RadialRainbow;});

    static std::string const ClassName() {return "RadialRainbow";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_cx_valueChanged(int);
    void on_cy_valueChanged(int);
    void on_shape_currentIndexChanged(int);

private:
    Ui::RadialRainbow *ui;
    double progress = 0.f;
    RGBColor GetColor(unsigned int, unsigned int, double, double, bool);
    unsigned int cx_shift = 50;
    unsigned int cy_shift = 50;
    unsigned int shape = 0;
};

#endif // RADIALRAINBOW_H
