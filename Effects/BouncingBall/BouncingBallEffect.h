#ifndef BOUNCINGBALLEFFECT_H
#define BOUNCINGBALLEFFECT_H

#include <unordered_map>
#include <unordered_set>

#include <QWidget>
#include "ColorUtils.h"
#include "BouncingBallSimulation.h"
#include "EffectRegisterer.h"
#include "hsv.h"
#include "RGBEffect.h"
#include "ui_BouncingBallEffect.h"

namespace Ui {
    class BouncingBallEffect;
}

class BouncingBallEffect: public RGBEffect
{
    Q_OBJECT

public:
    explicit BouncingBallEffect(QWidget *parent = nullptr);
    ~BouncingBallEffect();

    EFFECT_REGISTERER(ClassName(), [](){ return new BouncingBallEffect; });

    static std::string const ClassName() { return "BouncingBall"; }

    void SetFPS(unsigned int value) override;
    void SetBrightness(unsigned int value) override;
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;
    void OnControllerZonesListChanged(std::vector<ControllerZone*>) override;

private:
    Ui::BouncingBall *ui;

    void checkForDimensionChange(ControllerZone*, BouncingBallSimulation*);

    // Cannot use zone index as key, since preview controller registers with the
    // same id as the controller it's previewing. Using RGBController name instead.
    std::unordered_map<ControllerZone*, BouncingBallSimulation*> bouncingBallSimulations;

    int radius;
    int gravity;
    int dropHeightPercent;
    int spectrumVelocity;
    int horizontalVelocity;

private slots:
       void on_ball_radius_slider_valueChanged(int value);
       void on_gravity_slider_valueChanged(int value);
       void on_horizontal_velocity_slider_valueChanged(int value);
       void on_spectrum_velocity_slider_valueChanged(int value);
       void on_drop_height_percent_slider_valueChanged(int value);
       void on_reset_defaults_button_clicked();
};

#endif // BOUNCINGBALLEFFECT_H
