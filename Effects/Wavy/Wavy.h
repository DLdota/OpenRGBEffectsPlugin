#ifndef WAVY_H
#define WAVY_H

#include "RGBEffect.h"
#include "WavyUi.h"
#include <math.h>
#include <algorithm>

class Wavy: public QObject, public RGBEffect
{
    Q_OBJECT
public:
    Wavy(): QObject(nullptr){ GenerateRandomColors(); }
    ~Wavy() {};

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
    WavyUi *ui;
    bool   Dir = true ;
    float  SineProgress      = 0.0f;
    float  WaveProgress      = 0.0f;

    float  WaveFrequency     = 1;
    float  WaveSpeed         = 1;
    float  OscillationSpeed  = 1;
    bool   IsRandomColors    = false;

    std::vector<RGBColor>   UserColors;
    std::vector<RGBColor>   RandomColors;

    const double PI = std::atan(1)*4;

    RGBColor GetColor(int, int);
    RGBColor Interpolate(RGBColor color1, RGBColor color2, float fraction);
    void GenerateRandomColors();

private slots:
    void SetWaveFrequency(int);
    void SetWaveSpeed(int);
    void SetOscillationSpeed(int);
};

#endif // WAVY_H
