#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <QWidget>
#include "ui_Sequence.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "ColorPicker.h"

namespace Ui {
class Sequence;
}

class Sequence : public QWidget, public RGBEffect
{
    Q_OBJECT

public:
    explicit Sequence(QWidget *parent = nullptr);
    ~Sequence();

    EFFECT_REGISTERER(ClassName(), [](){return new Sequence;});

    static std::string const ClassName() {return "Sequence";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_colors_count_spinBox_valueChanged(int);

private:
    Ui::Sequence *ui;

    void ResetColors();
    ColorPicker* CreatePicker(int);
    RGBColor Interpolate(RGBColor, RGBColor, float);

    std::vector<RGBColor> colors;
    std::vector<ColorPicker*> color_pickers;

    double progress = 0.f;
};

#endif // SEQUENCE_H
