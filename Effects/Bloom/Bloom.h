#ifndef BLOOM_H
#define BLOOM_H

#include <QWidget>
#include "ui_Bloom.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"

namespace Ui {
class Bloom;
}

struct Flower {
    hsv_t hsv;
    float hue;
    float speed_mult = 1.f;
};

class Bloom : public RGBEffect
{
    Q_OBJECT

public:
    explicit Bloom(QWidget *parent = nullptr);
    ~Bloom();

    EFFECT_REGISTERER(ClassName(), [](){return new Bloom;});

    static std::string const ClassName() {return "Bloom";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void OnControllerZonesListChanged(std::vector<ControllerZone*>) override;

private:
    Ui::Bloom *ui;
    std::vector<std::vector<Flower>> flowers;

    void Reset(std::vector<ControllerZone*>);

    void UpdateFlowers(unsigned int);
};


#endif // BLOOM_H
