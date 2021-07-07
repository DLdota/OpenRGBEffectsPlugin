#ifndef BUBBLES_H
#define BUBBLES_H

#include "ui_Bubbles.h"
#include <QWidget>
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class Bubbles;
}

class Bubbles: public RGBEffect
{
    Q_OBJECT

public:
    explicit Bubbles(QWidget *parent = nullptr);
    ~Bubbles();

    EFFECT_REGISTERER(ClassName(), [](){return new Bubbles;});

    static std::string const ClassName() {return "Bubbles";}

    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private:
    Ui::Bubbles   *ui;

    std::vector<double>    speeds;
    std::vector<double>    bubbles;
    std::vector<int>       colors;
    std::vector<QPointF>   centers;

    RGBColor background;

    void InitBubble();
    void Cleanup();

    RGBColor GetColor(int, int, int, int);

    unsigned int max_bubbles = 10;
    unsigned int rarity = 50;
    unsigned int speed_mult = 1;
    unsigned int max_expansion = 100;
    unsigned int bubbles_thickness = 10;

private slots:
    void on_max_bubbles_valueChanged(int);
    void on_rarity_valueChanged(int);
    void on_max_expansion_valueChanged(int);
    void on_bubbles_thickness_valueChanged(int);
    void on_speed_mult_valueChanged(int);
    void on_background_ColorSelected(QColor);
};

#endif // BUBBLES_H
