#ifndef AUDIOSTAR_H
#define AUDIOSTAR_H

#include <QWidget>
#include "ui_AudioStar.h"
#include "ColorUtils.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "chuck_fft.h"

namespace Ui {
class AudioStar;
}

class AudioStar : public RGBEffect
{
    Q_OBJECT

public:
    explicit AudioStar(QWidget *parent = nullptr);
    ~AudioStar();

    EFFECT_REGISTERER(ClassName(), [](){return new AudioStar;});

    static std::string const ClassName() {return "AudioStar";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void EffectState(bool) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    void on_devices_currentIndexChanged(int);
    void on_amplitude_valueChanged(int);
    void on_decay_valueChanged(int);
    void on_edge_beat_stateChanged(int);

private:
    Ui::AudioStar *ui;

    float fft[256];
    unsigned char buffer[256];
    float win_hanning[256];
    float fft_nrml[256];
    float fft_fltr[256] = { 0 };
    float decay = 90.f;
    float filter_constant = 1.f;

    void Start();
    void Stop();

    int audio_device_idx = 0;
    float amplitude = 100.f;
    int avg_size = 8;
    bool edge_beat = true;

    RGBColor GetColor(float, float, float, float);

    double time = 0;
    double amp;
    float side = 1.f;
};


#endif // AUDIOSTAR_H
