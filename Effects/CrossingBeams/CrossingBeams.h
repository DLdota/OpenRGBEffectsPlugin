#ifndef CROSSINGBEAMS_H
#define CROSSINGBEAMS_H

#include <QWidget>
#include "ui_CrossingBeams.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"

namespace Ui {
class CrossingBeams;
}

class CrossingBeams : public RGBEffect
{
    Q_OBJECT

public:
    explicit CrossingBeams(QWidget *parent = nullptr);
    ~CrossingBeams();

    EFFECT_REGISTERER(ClassName(), [](){return new CrossingBeams;});

    static std::string const ClassName() {return "CrossingBeams";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;
    void SetUserColors(std::vector<RGBColor> colors) override;
    void SetRandomColorsEnabled(bool value) override;

private slots:
    void on_v_speed_valueChanged(int);
    void on_h_speed_valueChanged(int);
    void on_glow_valueChanged(int);
    void on_thickness_valueChanged(int);

private:
    Ui::CrossingBeams *ui;
    double progress = 0;
    double sine_x = 0;
    double sine_y = 0;

    int v_speed = 1;
    int h_speed = 1;
    int glow = 1;
    int thickness = 0;

    hsv_t hsv1;
    hsv_t hsv2;

    void ResetUserColors();
    RGBColor GetColor(unsigned int, unsigned int, unsigned int, unsigned int);

};

#endif // CROSSINGBEAMS_H
