#ifndef MARQUEE_H
#define MARQUEE_H

#include <QWidget>
#include "ui_Marquee.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"

namespace Ui {
class Marquee;
}

class Marquee : public RGBEffect
{
    Q_OBJECT

public:
    explicit Marquee(QWidget *parent = nullptr);
    ~Marquee();

    EFFECT_REGISTERER(ClassName(), CAT_SIMPLE, [](){return new Marquee;});

    static std::string const ClassName() {return "Marquee";}
    void StepEffect(std::vector<ControllerZone*>) override;

private:
    Ui::Marquee *ui;
    RGBColor GetColor(unsigned int);
    double progress = 0.0;

    hsv_t random;
};

#endif // MARQUEE_H
