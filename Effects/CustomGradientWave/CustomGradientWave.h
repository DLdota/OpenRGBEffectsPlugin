#ifndef CUSTOMGRADIENTWAVE_H
#define CUSTOMGRADIENTWAVE_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include <QWidget>
#include "ColorPicker.h"
#include "ui_CustomGradientWave.h"
#include "ColorUtils.h"

enum
{
    HORIZONTAL = 0,
    VERTICAL   = 1,
    RADIAL_OUT = 2,
    RADIAL_IN  = 3
};

typedef struct
{
    std::string name;
    std::vector<RGBColor> colors;

} CustomGradientWavePreset;

namespace Ui {
class CustomGradientWave;
}

class CustomGradientWave : public RGBEffect
{
    Q_OBJECT

public:
    explicit CustomGradientWave(QWidget *parent = nullptr);
    ~CustomGradientWave();

    EFFECT_REGISTERER(ClassName(), CAT_RAINBOW, [](){return new CustomGradientWave;});

    static std::string const ClassName() {return "CustomGradientWave";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_colors_count_spinBox_valueChanged(int);
    void on_preset_currentTextChanged(const QString&);
    void on_spread_valueChanged(int);
    void on_direction_currentIndexChanged(int);

private:
    Ui::CustomGradientWave *ui;
    double progress = 0.0;
    RGBColor GetColor(float, float, float, float);
    void ResetColors();
    ColorPicker* CreatePicker(int);
    void LoadPreset(const QString&);

    std::vector<RGBColor> colors;
    std::vector<ColorPicker*> color_pickers;
    QImage gradient;
    void GenerateGradient();

    int spread    = 0;
    int direction = HORIZONTAL;

    std::vector<CustomGradientWavePreset> presets =
    {
        {"UnicornVomit", std::vector<RGBColor>{
             HEXCOLOR(0xff0004),
             HEXCOLOR(0xff00e6),
             HEXCOLOR(0x0000ff),
             HEXCOLOR(0x00b3ff),
             HEXCOLOR(0x00ff51),
             HEXCOLOR(0xeaff00),
             HEXCOLOR(0xffb300),
             HEXCOLOR(0xff0000)
         }},

        {"Pink/Blue", std::vector<RGBColor>{
             HEXCOLOR(0xfe00c5),
             HEXCOLOR(0x00c5ff),
             HEXCOLOR(0x00c5ff),
             HEXCOLOR(0xfe00c5)
         }},

        {"Pink/Gold", std::vector<RGBColor>{
             HEXCOLOR(0xfee000),
             HEXCOLOR(0xfe00fe),
             HEXCOLOR(0xfe00fe),
             HEXCOLOR(0xfee000),
         }},

        {"Purple/Orange", std::vector<RGBColor>{
             HEXCOLOR(0xff2100),
             HEXCOLOR(0xaa00ff),
             HEXCOLOR(0xaa00ff),
             HEXCOLOR(0xff2100),
             HEXCOLOR(0xff2100),
             HEXCOLOR(0xff2100)
         }},

        {"LightBlue/Purple", std::vector<RGBColor>{
             HEXCOLOR(0x03fffa),
             HEXCOLOR(0x55007f),
             HEXCOLOR(0x55007f),
             HEXCOLOR(0x03fffa)
         }},

        {"PoliceBeacon", std::vector<RGBColor>{
             HEXCOLOR(0xff0000),
             HEXCOLOR(0x0000ff),
             HEXCOLOR(0x0000ff),
             HEXCOLOR(0xff0000),
             HEXCOLOR(0xff0000)
         }},

        {"Seabed", std::vector<RGBColor>{
             HEXCOLOR(0x00ff00),
             HEXCOLOR(0x0032ff),
             HEXCOLOR(0x0032ff),
             HEXCOLOR(0x00ff00),
             HEXCOLOR(0x00ff00)
         }},

        {"Sunset", std::vector<RGBColor>{
             HEXCOLOR(0xff2100),
             HEXCOLOR(0xab006d),
             HEXCOLOR(0xc01c52),
             HEXCOLOR(0xd53737),
             HEXCOLOR(0xea531b),
             HEXCOLOR(0xff6e00),
             HEXCOLOR(0xff0000),
             HEXCOLOR(0xff2100)
         }}
    };
};

#endif // CUSTOMGRADIENTWAVE_H
