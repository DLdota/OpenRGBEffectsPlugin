#ifndef WAVY_H
#define WAVY_H


#include "ui_Wavy.h"
#include <QWidget>
#include "RGBEffect.h"

#include <math.h>
#include <algorithm>

namespace Ui {
class Wavy;
}

class Wavy: public QWidget, public RGBEffect
{
    Q_OBJECT

public:
    explicit Wavy(QWidget *parent = nullptr);
    ~Wavy();

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

    void                    EffectState(bool)                               override {return;            };

    void                    LoadCustomSettings(json)                        override;
    json                    SaveCustomSettings(json)                        override;

    EffectInfo EffectDetails;

private:
    Ui::Wavy   *ui;
    bool       Dir = true ;
    float      SineProgress      = 0.0f;
    float      WaveProgress      = 0.0f;

    float      WaveFrequency     = 1;
    float      WaveSpeed         = 1;
    float      OscillationSpeed  = 1;
    bool       IsRandomColors    = false;

    std::vector<RGBColor>   UserColors;
    std::vector<RGBColor>   RandomColors;

    const double PI = std::atan(1)*4;

    RGBColor GetColor(int, int);
    RGBColor Interpolate(RGBColor color1, RGBColor color2, float fraction);
    void GenerateRandomColors();

private slots:
    void on_wave_freq_slider_valueChanged(int NewVal);
    void on_wave_speed_slider_valueChanged(int NewVal);
    void on_oscillation_speed_slider_valueChanged(int NewVal);
};

#endif // WAVY_H
