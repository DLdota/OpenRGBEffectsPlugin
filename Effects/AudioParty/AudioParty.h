#ifndef AUDIOPARTY_H
#define AUDIOPARTY_H

#include <QWidget>
#include "ui_AudioParty.h"
#include "ColorUtils.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "chuck_fft.h"
#include "ctkrangeslider.h"

namespace Ui {
class AudioParty;
}

class AudioParty : public RGBEffect
{
    Q_OBJECT

public:
    explicit AudioParty(QWidget *parent = nullptr);
    ~AudioParty();

    EFFECT_REGISTERER(ClassName(), CAT_AUDIO, [](){return new AudioParty;});

    static std::string const ClassName() {return "AudioParty";}
    void StepEffect(std::vector<ControllerZone*>) override;
    void EffectState(bool) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private slots:
    void on_devices_currentIndexChanged(int);
    void on_amplitude_valueChanged(int);
    void on_divisions_valueChanged(int);
    void on_effect_threshold_valueChanged(double);
    void ZonesChanged(int, int);
    void UpdateGraph(QPixmap);

signals:
    void UpdateGraphSignal(QPixmap) const;

private:
    Ui::AudioParty *ui;

    float fft[256];
    unsigned char buffer[256];
    float win_hanning[256];
    float fft_nrml[256];
    float fft_fltr[256] = { 0 };
    float decay = 85.f;
    float filter_constant = 1.f;

    void Start();
    void Stop();

    double x_shift = 0.f;
    double color_shift = 0.f;

    int audio_device_idx = 0;
    float divisions = 2.0;
    float effect_threshold = 0.2;
    float amplitude = 100.f;
    unsigned int avg_size = 8;
    unsigned int motion_zone_stop = 8;
    unsigned int color_zone_stop = 24;

    RGBColor background = ColorUtils::OFF();
    RGBColor wave_color = ToRGBColor(0,255,0);

    RGBColor GetColor(float, float, float, float);

    RGBColor color;

    double effect_progress = 1.0;
    unsigned int effect_idx = 0;

    ctkRangeSlider* zones_slider;

};

#endif // AUDIOPARTY_H
