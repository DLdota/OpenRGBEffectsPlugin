#ifndef MOSAIC_H
#define MOSAIC_H

#include <QWidget>
#include "ui_Mosaic.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"

namespace Ui {
class Mosaic;
}

struct Tile {
    hsv_t hsv;
    float brightness = 0.f;
    float decrease_speed_mult = 1.f;
};

class Mosaic : public RGBEffect
{
    Q_OBJECT

public:
    explicit Mosaic(QWidget *parent = nullptr);
    ~Mosaic();

    EFFECT_REGISTERER(ClassName(), CAT_BASIC, [](){return new Mosaic;});

    static std::string const ClassName() {return "Mosaic";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void OnControllerZonesListChanged(std::vector<ControllerZone*>) override;

private:
    Ui::Mosaic *ui;

    void ResetMosaic(std::vector<ControllerZone*>);

    void UpdateTiles(unsigned int);

    std::vector<std::vector<Tile>> tiles;
};

#endif // MOSAIC_H
