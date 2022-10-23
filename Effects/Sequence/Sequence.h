#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <QWidget>
#include "ui_Sequence.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class Sequence;
}

class Sequence : public RGBEffect
{
    Q_OBJECT

public:
    explicit Sequence(QWidget *parent = nullptr);
    ~Sequence();

    EFFECT_REGISTERER(ClassName(), CAT_ADVANCED, [](){return new Sequence;});

    static std::string const ClassName() {return "Sequence";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private:
    Ui::Sequence *ui;

    double progress = 0.f;
};

#endif // SEQUENCE_H
