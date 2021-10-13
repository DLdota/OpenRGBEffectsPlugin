#ifndef SPIRAL_H
#define SPIRAL_H

#include <QWidget>
#include "ui_Spiral.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class Spiral;
}

class Spiral : public RGBEffect
{
    Q_OBJECT

public:
    explicit Spiral(QWidget *parent = nullptr);
    ~Spiral();

    EFFECT_REGISTERER(ClassName(), [](){return new Spiral;});

    static std::string const ClassName() {return "Spiral";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;

private:
    Ui::Spiral *ui;

    double time = 1000.0;

    RGBColor GetColor(float, float, float, float, bool);
};

#endif // SPIRAL_H
