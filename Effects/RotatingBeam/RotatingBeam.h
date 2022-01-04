#ifndef ROTATINGBEAM_H
#define ROTATINGBEAM_H

#include <QWidget>
#include "ui_RotatingBeam.h"
#include "hsv.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class RotatingBeam;
}

class RotatingBeam : public RGBEffect
{
    Q_OBJECT

public:
    explicit RotatingBeam(QWidget *parent = nullptr);
    ~RotatingBeam();

    EFFECT_REGISTERER(ClassName(), CAT_BASIC, [](){return new RotatingBeam;});

    static std::string const ClassName() {return "RotatingBeam";}

    void DefineExtraOptions(QLayout*)  override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void SetUserColors(std::vector<RGBColor> colors) override;
    void SetRandomColorsEnabled(bool value) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_mode_currentIndexChanged(int);
    void on_thickness_valueChanged(int);

private:
    Ui::RotatingBeam *ui;

    double progress = 0.0;

    int thickness = 0;

    RGBColor GetColor(float, float, QLineF, float, float);

    hsv_t hsv1;
    hsv_t hsv2;

    void ResetUserColors();

    enum RotatingBeamMode
    {
        CLOCKWISE = 0,
        COUNTER_CLOCKWISE = 1,
        PENDULUM = 2,
        WIPERS = 3,
        SWING_H = 4,
        SWING_V = 5
    };

    QStringList modes = QStringList(
                {
                    "Clockwise",
                    "Counter clockwise",
                    "Pendulum",
                    "Wipers",
                    "Swing H",
                    "Swing V"
                });

    RotatingBeamMode mode = CLOCKWISE;

    QLineF Rotate();
};

#endif // ROTATINGBEAM_H
