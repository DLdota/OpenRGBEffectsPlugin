#ifndef AUDIOSINE_H
#define AUDIOSINE_H

#include "AudioSignalProcessor.h"
#include "AudioSettings.h"
#include "AudioSettingsStruct.h"

#include <QWidget>
#include "ui_AudioSine.h"
#include "ColorUtils.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class AudioSine;
}

class AudioSine : public RGBEffect
{
    Q_OBJECT

public:
    explicit AudioSine(QWidget *parent = nullptr);
    ~AudioSine();

    EFFECT_REGISTERER(ClassName(), CAT_AUDIO, [](){return new AudioSine;});

    static std::string const ClassName() {return "AudioSine";}
    void StepEffect(std::vector<ControllerZone*>) override;
    void EffectState(bool) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private slots:
    void on_color_mode_currentIndexChanged(int);    
    void on_repeat_valueChanged(int);
    void on_glow_valueChanged(int);
    void on_thickness_valueChanged(int);    
    void on_oscillation_valueChanged(int);
    void on_color_change_speed_valueChanged(int);
    void on_background_ColorSelected(QColor);
    void on_wave_color_ColorSelected(QColor);

    void on_audio_settings_clicked();
    void OnAudioDeviceChanged(int);

private:
    Ui::AudioSine*  ui;

    double          x_time = 0.f;
    double          oscillation_time = 0.f;
    double          color_time = 0.f;
    double          height_mult = 0.f;
    int             repeat = 1;
    int             glow = 1;
    int             thickness = 0;
    int             oscillation = 0;
    int             color_change_speed = 0;
    int             color_mode = 0;

    RGBColor        background = ColorUtils::OFF();
    RGBColor        wave_color = ToRGBColor(0,255,0);

    AudioSettings                   audio_settings;
    Audio::AudioSettingsStruct      audio_settings_struct;
    AudioSignalProcessor            audio_signal_processor;

    void            Start();
    void            Stop();
    float           GetSineValue(unsigned int, unsigned int, bool);
    RGBColor        GetColor(float, unsigned int, unsigned int);
};

#endif // AUDIOSINE_H
