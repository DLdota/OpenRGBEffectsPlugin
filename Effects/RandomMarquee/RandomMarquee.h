#ifndef RANDOM_MARQUEE_H
#define RANDOM_MARQUEE_H

#include <QWidget>
#include "ui_RandomMarquee.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"

namespace Ui {
class RandomMarquee;
}

class RandomMarquee : public RGBEffect
{
    Q_OBJECT

public:
    explicit RandomMarquee(QWidget *parent = nullptr);
    ~RandomMarquee();

    EFFECT_REGISTERER(ClassName(), CAT_RANDOM, [](){return new RandomMarquee;});

    static std::string const ClassName() {return "RandomMarquee";}
    void StepEffect(std::vector<ControllerZone*>) override;

private:
    Ui::RandomMarquee *ui;
    RGBColor GetColor(unsigned int);
    double custom_rand(double min, double max);

    double progress = 0.0;
    unsigned int last_progress = 0;
    hsv_t random;
    unsigned int spacing = 1;
    double speed_mult = 0.5;
    double progress_mult = 0.5;
    bool dir = false;
};

#endif // MARQUEE_H
