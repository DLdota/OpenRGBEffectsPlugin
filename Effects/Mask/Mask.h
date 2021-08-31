#ifndef MASK_H
#define MASK_H

#include <QWidget>
#include "ui_Mask.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class Mask;
}

class Mask : public RGBEffect
{
    Q_OBJECT

public:
    explicit Mask(QWidget *parent = nullptr);
    ~Mask();

    EFFECT_REGISTERER(ClassName(), [](){return new Mask;});

    static std::string const ClassName() {return "Mask";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_x_valueChanged(double);
    void on_y_valueChanged(double);
    void on_w_valueChanged(double);
    void on_h_valueChanged(double);

private:
    Ui::Mask *ui;
    RGBColor GetColor(float, float, float, float);
    QRectF restrict = QRectF(0.f, 0.f, 1.f, 1.f);
};

#endif // MASK_H