#ifndef CUSTOMMARQUEE_H
#define CUSTOMMARQUEE_H

#include <QWidget>
#include "ui_CustomMarquee.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class CustomMarquee;
}

class CustomMarquee : public RGBEffect
{
    Q_OBJECT

public:
    explicit CustomMarquee(QWidget *parent = nullptr);
    ~CustomMarquee();

    EFFECT_REGISTERER(ClassName(), CAT_ADVANCED, [](){return new CustomMarquee;});

    static std::string const ClassName() {return "CustomMarquee";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private:
    Ui::CustomMarquee *ui;
    RGBColor GetColor(unsigned int);
    double progress = 0.0;

};

#endif // CUSTOMMARQUEE_H
