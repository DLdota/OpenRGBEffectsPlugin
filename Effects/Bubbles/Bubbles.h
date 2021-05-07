#ifndef BUBBLES_H
#define BUBBLES_H

#include "ui_Bubbles.h"
#include <QWidget>
#include "RGBEffect.h"

namespace Ui {
class Bubbles;
}

class Bubbles: public QWidget, public RGBEffect
{
    Q_OBJECT

public:
    explicit Bubbles(QWidget *parent = nullptr);
    ~Bubbles();

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QLayout*)                                override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int)       override;
    void        SetSpeed(int)                                               override {};
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override {};
    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>)  override {};
    void        ToggleRandomColors(bool)                                    override;

    int                     GetSpeed()                                      override {return 0;          };
    int                     GetSlider2Val()                                 override {return 0;          };
    std::vector<RGBColor>   GetUserColors()                                 override {return UserColors; };

    void                    EffectState(bool)                               override {};

    void                    LoadCustomSettings(json)                        override;
    json                    SaveCustomSettings(json)                        override;

    EffectInfo EffectDetails;

private:
    Ui::Bubbles   *ui;

    bool random_enabled = false;

    std::vector<double>    speeds;
    std::vector<double>    bubbles;
    std::vector<int>       colors;
    std::vector<QPointF>   centers;

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
};

#endif // BUBBLES_H
