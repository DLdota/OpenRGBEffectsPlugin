#ifndef SMOOTHBLINK_H
#define SMOOTHBLINK_H

#include "ui_SmoothBlink.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

#include <QWidget>

namespace Ui {
class SmoothBlink;
}

class SmoothBlink : public RGBEffect
{
    Q_OBJECT

public:
    explicit SmoothBlink(QWidget *parent = nullptr);
    ~SmoothBlink();

    EFFECT_REGISTERER(ClassName(), [](){return new SmoothBlink;});

    static std::string const ClassName() {return "SmoothBlink";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:

    void on_interval_valueChanged(double);
    void on_pulses_valueChanged(int);
    void on_pulse_duration_valueChanged(double);
    void on_strength_valueChanged(double);
    void on_defaults_clicked();
    void on_rendering_currentIndexChanged(int);
    void on_cx_valueChanged(int);
    void on_cy_valueChanged(int);

private:
    Ui::SmoothBlink *ui;    

    void Defaults();

    double time = 0;
    double random_fade_timer = 0;

    RGBColor current_color;
    RGBColor random_color;
    RGBColor next_color;

    enum PulseMode {
        Saturation = 0,
        Brightness = 1
    };

    enum PulseRendering {
        Solid = 0,
        Circle = 1,
        Square = 2
    };

    double pi = 3.14159265358979323846;

    float default_interval = 2.0;
    float default_strength = 0.5;
    unsigned int default_pulses = 2;
    float default_pulse_duration = 0.5;
    PulseRendering default_rendering = Solid;

    float interval = default_interval;
    float strength = default_strength;
    unsigned int pulses = default_pulses;
    float pulse_duration = default_pulse_duration;
    PulseRendering rendering = default_rendering;

    unsigned int cx_shift = 50;
    unsigned int cy_shift = 50;

    float pulses_total_duration;
    float total_effect_duration;
    float value = 1.0;

    RGBColor GetColor(unsigned int, unsigned int, double, double, unsigned int);

    void HandleSolidRendering(std::vector<ControllerZone*>);
    void HandleCircleRendering(std::vector<ControllerZone*>);

};

#endif // SMOOTHBLINK_H
