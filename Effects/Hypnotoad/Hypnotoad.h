#ifndef HYPNOTOAD_H
#define HYPNOTOAD_H

#include <QWidget>
#include "ui_Hypnotoad.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class Hypnotoad;
}

class Hypnotoad : public RGBEffect
{
    Q_OBJECT

public:
    explicit Hypnotoad(QWidget *parent = nullptr);
    ~Hypnotoad();

    EFFECT_REGISTERER(ClassName(), CAT_BASIC, [](){return new Hypnotoad;});

    static std::string const ClassName() {return "Hypnotoad";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_animation_speed_valueChanged(int);
    void on_color_rotation_speed_valueChanged(int);
    void on_animation_direction_currentIndexChanged(int);
    void on_color_rotation_direction_currentIndexChanged(int);    
    void on_spacing_valueChanged(int);
    void on_thickness_valueChanged(int);
    void on_cx_valueChanged(int);
    void on_cy_valueChanged(int);

private:
    Ui::Hypnotoad *ui;
    double progress = 1000.0;
    RGBColor GetColor(unsigned int, unsigned int, float, float, bool);

    float animation_speed = 10.f;
    float color_rotation_speed = 10.f;
    unsigned int animation_direction = 0;
    unsigned int color_rotation_direction = 0;
    unsigned int spacing = 1;
    unsigned int thickness = 1;
    unsigned int cx_shift = 50;
    unsigned int cy_shift = 50;

};

#endif // HYPNOTOAD_H
