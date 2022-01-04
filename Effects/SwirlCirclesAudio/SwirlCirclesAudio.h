#ifndef SWIRLCIRCLESAUDIO_H
#define SWIRLCIRCLESAUDIO_H


#include <QWidget>
#include "ui_SwirlCirclesAudio.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"
#include "chuck_fft.h"

namespace Ui {
class SwirlCirclesAudio;
}

class SwirlCirclesAudio : public RGBEffect
{
    Q_OBJECT

public:
    explicit SwirlCirclesAudio(QWidget *parent = nullptr);
    ~SwirlCirclesAudio();

    EFFECT_REGISTERER(ClassName(), CAT_AUDIO, [](){return new SwirlCirclesAudio;});

    static std::string const ClassName() {return "SwirlCirclesAudio";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void SetUserColors(std::vector<RGBColor> colors) override;
    void SetRandomColorsEnabled(bool value) override;
    void EffectState(bool) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_devices_currentIndexChanged(int);
    void on_amplitude_valueChanged(int);
    void on_decay_valueChanged(int);
    void on_average_valueChanged(int);
    void on_radius_valueChanged(int);

private:
    Ui::SwirlCirclesAudio *ui;
    double progress = 0.0;

    void ResetUserColors();

    hsv_t hsv1;
    hsv_t hsv2;

    RGBColor GetColor(unsigned int, unsigned int,  unsigned int,  unsigned int, float, float);

    float fft[256];
    unsigned char buffer[256];
    float win_hanning[256];
    float fft_nrml[256];
    float fft_fltr[256];
    float filter_constant = 1.f;

    int audio_device_idx = 0;
    float amplitude = 100.f;
    float decay = 85.f;
    int avg_size = 32;
    int radius = 0;

    float current_level = 0.f;

    void Start();
    void Stop();
};

#endif // SWIRLCIRCLESAUDIO_H
