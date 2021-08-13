#ifndef SWAP_H
#define SWAP_H

#include <QWidget>
#include "ui_Swap.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class Swap;
}

class Swap : public RGBEffect
{
    Q_OBJECT

public:
    explicit Swap(QWidget *parent = nullptr);
    ~Swap();

    EFFECT_REGISTERER(ClassName(), [](){return new Swap;});

    static std::string const ClassName() {return "Swap";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;

private:
    Ui::Swap *ui;

    RGBColor GetColor(unsigned int, unsigned int);

    RGBColor c1;
    RGBColor c2;

    RGBColor random1;
    RGBColor random2;


    double time = 0.0;
    double progress = 0.0;
    bool dir = 0;
    bool old_dir = 0;

};

#endif // SWAP_H
