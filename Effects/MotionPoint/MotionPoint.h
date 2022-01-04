#ifndef MOTIONPOINT_H
#define MOTIONPOINT_H

#include <QWidget>
#include "ui_MotionPoint.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "ColorUtils.h"

namespace Ui {
class MotionPoint;
}

class MotionPoint : public RGBEffect
{
    Q_OBJECT

public:
    explicit MotionPoint(QWidget *parent = nullptr);
    ~MotionPoint();

    EFFECT_REGISTERER(ClassName(), CAT_SIMPLE, [](){return new MotionPoint;});

    static std::string const ClassName() {return "MotionPoint";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_background_ColorSelected(QColor);

private:
    Ui::MotionPoint *ui;

    RGBColor GetColor(unsigned int, unsigned int, double);

    double progress = 0.f;
    RGBColor background = ColorUtils::OFF();
    RGBColor current = ColorUtils::OFF();

};

#endif // MOTIONPOINT_H
