#ifndef SPECTRUMCYCLING_H
#define SPECTRUMCYCLING_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include <QWidget>

namespace Ui {
class SpectrumCycling;
}
class SpectrumCycling : public RGBEffect
{
    Q_OBJECT

public:
    explicit SpectrumCycling(QWidget *parent = nullptr);
    ~SpectrumCycling();

    EFFECT_REGISTERER(ClassName(), CAT_RAINBOW, [](){return new SpectrumCycling;});

    static std::string const ClassName() {return "SpectrumCycling";}

    void StepEffect(std::vector<ControllerZone*>) override;

    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private slots:
    void on_saturation_valueChanged(int);

private:
    Ui::SpectrumCycling *ui;
    double progress = 0.0;
    int  saturation   = 255;
};

#endif // SPECTRUMCYCLING_H
