#ifndef AUDIOVUMETER_H
#define AUDIOVUMETER_H

#include <QWidget>
#include "ui_AudioVUMeter.h"
#include "ColorUtils.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "chuck_fft.h"

namespace Ui {
class AudioVUMeter;
}

class AudioVUMeter : public RGBEffect
{
    Q_OBJECT

public:
    explicit AudioVUMeter(QWidget *parent = nullptr);
    ~AudioVUMeter();

    EFFECT_REGISTERER(ClassName(), CAT_AUDIO, [](){return new AudioVUMeter;});

    static std::string const ClassName() {return "AudioVUMeter";}

    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void EffectState(bool) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_devices_currentIndexChanged(int);
    void on_amplitude_valueChanged(int);
    void on_average_valueChanged(int);
    void on_decay_valueChanged(int);
    void on_color_offset_valueChanged(int);
    void on_color_spread_valueChanged(int);
    void on_saturation_valueChanged(int);

private:
    Ui::AudioVUMeter *ui;

    float fft[256];
    unsigned char buffer[256];
    float win_hanning[256];
    float fft_nrml[256];
    float fft_fltr[256];
    float decay = 85.f;
    float filter_constant = 1.f;

    float last_height = 0.f;

    float color_offset = 0.f;
    float color_spread = 0.f;
    int   saturation   = 255;

    void Start();
    void Stop();

    int audio_device_idx = 0;
    float amplitude = 100.f;
    int avg_size = 1;

    RGBColor GetColor(float, float, float);
};

#endif // AUDIOVUMETER_H
