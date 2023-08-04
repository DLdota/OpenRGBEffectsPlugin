#ifndef RANDOM_SPIN_H
#define RANDOM_SPIN_H

#include <QWidget>
#include "ui_RandomSpin.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class RandomSpin;
}

struct RandomSpinEntry
{
    double  stop_progress   = 0;
    double  progress        = 0;
    double  speed_mult      = 1.f;
    bool    dir             = true;
    bool    stop            = true;
    double  next_time_point = 0.f;
};

class RandomSpin : public RGBEffect
{
    Q_OBJECT

public:
    explicit RandomSpin(QWidget *parent = nullptr);
    ~RandomSpin();

    EFFECT_REGISTERER(ClassName(), CAT_RANDOM, [](){return new RandomSpin;});

    static std::string const ClassName() {return "RandomSpin";}
    void StepEffect(std::vector<ControllerZone*>) override;
    void OnControllerZonesListChanged(std::vector<ControllerZone*>)  override;
    void SetUserColors(std::vector<RGBColor> colors) override;

private:
    Ui::RandomSpin *ui;
    RGBColor GetColor(unsigned int i, unsigned int w, const RandomSpinEntry& entry);
    double custom_rand(double, double);

    double progress = 0.0;
    std::vector<RandomSpinEntry> spin_entries;
    QImage gradient;
    void GenerateGradient();
};

#endif // MARQUEE_H
