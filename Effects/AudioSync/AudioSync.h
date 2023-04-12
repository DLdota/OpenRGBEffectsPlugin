#ifndef AUDIOSYNC_H
#define AUDIOSYNC_H

#include "AudioSignalProcessor.h"
#include "AudioSettings.h"
#include "AudioSettingsStruct.h"

#include <QLayout>
#include <QComboBox>
#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QTimer>

#include "ui_AudioSync.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

enum SaturationMode
{
    NO_SATURATION = 0,
    SATURATE_HIGH_AMPLITUDES = 1,
    B_W_MODE = 2
};

enum RollMode
{
    LINEAR = 0,
    NONE = 1,
    RADIAL = 2
};

namespace Ui {
    class AudioSync;
}

class AudioSync: public RGBEffect
{
    Q_OBJECT

public:
    explicit AudioSync(QWidget *parent = nullptr);
    ~AudioSync();

    EFFECT_REGISTERER(ClassName(), CAT_AUDIO, [](){return new AudioSync;});

    static std::string const ClassName() {return "AudioSync";}

    void StepEffect(std::vector<ControllerZone*>)               override;
    void LoadCustomSettings(json)                               override;
    json SaveCustomSettings()                                   override;
    void EffectState(bool)                                      override;

private slots:
    void OnAudioDeviceChanged(int);
    void UpdateGraph(QPixmap);

    void on_audio_settings_clicked();
    void on_color_fade_speed_valueChanged(int);
    void on_hue_shift_valueChanged(int);
    void on_bypass_valuesChanged(int,int);
    void on_saturation_currentIndexChanged(int);
    void on_roll_mode_currentIndexChanged(int);

signals:
    void UpdateGraphSignal(QPixmap) const;

private:    
    Ui::AudioSync *ui;

    /*-----*\
    | Ui    |
    \*-----*/
    QFrame* PrimaryFrame;
    QFrame* AudioSyncFrame;

    /*----------*\
    | internals  |
    \*----------*/
    int                     immediate_freq_hue = 0;
    int                     current_freq_hue = 0;
    float                   current_freq_sat = 0;
    float                   current_freq_val = 0;
    std::vector<int>        rainbow_hues;
    std::vector<RGBColor>   colors_rotation;
    int                     fade_step;
    int                     hue_shift;
    int                     bypass_min;
    int                     bypass_max;
    int                     saturation_mode;
    int                     roll_mode;

    AudioSettings                   audio_settings;
    Audio::AudioSettingsStruct      audio_settings_struct;
    AudioSignalProcessor            audio_signal_processor;

    /*--------*\
    | Methods  |
    \*--------*/
    void Start();
    void Stop();
    void CreateUi();
    RGBColor GetColor(int, int, int, int);


};

#endif // AUDIOSYNC_H
