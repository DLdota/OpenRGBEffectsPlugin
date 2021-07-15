#ifndef SWIRLCIRCLES_H
#define SWIRLCIRCLES_H

#include <QWidget>
#include "ui_SwirlCircles.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"


namespace Ui {
class SwirlCircles;
}

class SwirlCircles : public RGBEffect
{
    Q_OBJECT

public:
    explicit SwirlCircles(QWidget *parent = nullptr);
    ~SwirlCircles();

    EFFECT_REGISTERER(ClassName(), [](){return new SwirlCircles;});

    static std::string const ClassName() {return "SwirlCircles";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void SetUserColors(std::vector<RGBColor> colors) override;
    void SetRandomColorsEnabled(bool value) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_radius_valueChanged(int);

private:
    Ui::SwirlCircles *ui;
    double progress = 0.0;
    int radius = 0;

    void ResetUserColors();

    hsv_t hsv1;
    hsv_t hsv2;

    RGBColor GetColor(unsigned int, unsigned int,  unsigned int,  unsigned int, float, float);

};

#endif // SWIRLCIRCLES_H
