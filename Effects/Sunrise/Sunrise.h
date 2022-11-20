#ifndef SUNRISE_H
#define SUNRISE_H

#include <QWidget>
#include "ui_Sunrise.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "ColorUtils.h"

namespace Ui {
class Sunrise;
}

class Sunrise : public RGBEffect
{
    Q_OBJECT

public:
    explicit Sunrise(QWidget *parent = nullptr);
    ~Sunrise();

    EFFECT_REGISTERER(ClassName(), CAT_ADVANCED, [](){return new Sunrise;});

    static std::string const ClassName() {return "Sunrise";}
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private slots:
   void on_max_intensity_valueChanged(int);
   void on_intensity_speed_valueChanged(int);
   void on_radius_valueChanged(int);
   void on_grow_speed_valueChanged(int);
   void on_motion_stateChanged(int);
   void on_run_once_stateChanged(int);

private:
    Ui::Sunrise *ui;

    double time = 0;
    double progress = 0;
    double y_shift = -1;

    RGBColor GetColor(float, float, float, float);
    QImage image = QImage(100, 1, QImage::Format_RGB32);

    int max_intensity = 80; // 1 - 100
    int intensity_speed = 10; // 1 - 100
    bool motion = false; // true - false
    int radius = 50; // 1 - 100
    int grow_speed = 10; // 1 - 20
    bool run_once = false;

};

#endif // SUNRISE_H
