#ifndef NOISEMAP_H
#define NOISEMAP_H

#include <QWidget>
#include <QImage>
#include "ui_NoiseMap.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "SimplexNoise.h"
#include "ColorPicker.h"

namespace Ui {
class NoiseMap;
}

class NoiseMap : public RGBEffect
{
    Q_OBJECT

public:
    explicit NoiseMap(QWidget *parent = nullptr);
    ~NoiseMap();    

    EFFECT_REGISTERER(ClassName(), [](){return new NoiseMap;});

    static std::string const ClassName() {return "NoiseMap";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_amplitude_valueChanged(int);
    void on_frequency_valueChanged(int);
    void on_lacunarity_valueChanged(int);
    void on_persistence_valueChanged(int);
    void on_octaves_valueChanged(int);
    void on_motion_speed_valueChanged(int);
    void on_motion_currentIndexChanged(int);

    void on_defaults_clicked();

    void on_colors_count_spinBox_valueChanged(int);
    void on_colors_choice_currentIndexChanged(int);

private:
    Ui::NoiseMap *ui;

    void Defaults();
    void ResetNoise();
    RGBColor GetColor(unsigned int, unsigned int);

    float val_mult = 10000;
    float inv_val_mult = 1.f / val_mult;

    float default_frequency = 0.12;
    float default_amplitude = 3.9;
    float default_lacunarity = 0.75;
    float default_persistence = 0.5;
    int default_octaves = 2;
    int default_motion = 0;
    int default_motion_speed = 0;

    float frequency = default_frequency;
    float amplitude = default_amplitude;
    float lacunarity = default_lacunarity;
    float persistence = default_persistence;
    unsigned int octaves = default_octaves;
    unsigned int motion = default_motion;
    unsigned int motion_speed = default_motion_speed;

    SimplexNoise* noise;

    double progress = 0.f;

    void ResetColors();
    void GenerateGradient();
    ColorPicker* CreatePicker(int);
    std::vector<RGBColor> colors;
    std::vector<ColorPicker*> color_pickers;
    QImage image = QImage(100, 1, QImage::Format_RGB32);
};

#endif // NOISEMAP_H
