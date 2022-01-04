#ifndef COMET_H
#define COMET_H

#include <QWidget>
#include "ui_Comet.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"

namespace Ui {
class Comet;
}

class Comet : public RGBEffect
{
    Q_OBJECT

public:
    explicit Comet(QWidget *parent = nullptr);
    ~Comet();

    EFFECT_REGISTERER(ClassName(), CAT_BASIC, [](){return new Comet;});

    static std::string const ClassName() {return "Comet";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;

private:
    Ui::Comet *ui;
    double time = 0;
    double progress = 0;

    RGBColor GetColor(unsigned int, unsigned int);

    hsv_t tmp;

};

#endif // COMET_H
