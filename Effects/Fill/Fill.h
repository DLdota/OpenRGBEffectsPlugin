#ifndef FILL_H
#define FILL_H

#include <QWidget>
#include "ui_Fill.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class Fill;
}

class Fill : public RGBEffect
{
    Q_OBJECT

public:
    explicit Fill(QWidget *parent = nullptr);
    ~Fill();

    EFFECT_REGISTERER(ClassName(), CAT_SIMPLE, [](){return new Fill;});

    static std::string const ClassName() {return "Fill";}
    void StepEffect(std::vector<ControllerZone*>) override;

private:
    Ui::Fill *ui;
    double time = 0;
    double progress = 0;
    double old_progress = 0;
    RGBColor random;

    RGBColor GetColor(unsigned int, unsigned int);
};

#endif // FILL_H
