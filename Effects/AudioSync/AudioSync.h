#ifndef AUDIOSYNC_H
#define AUDIOSYNC_H

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

struct AudioSyncSettings
{
    std::string name;

    int     fade_step;
    int     rainbow_shift;
    int     bypass_min;
    int     bypass_max;
    int     avg_size;
    int     avg_mode;
    int     saturation_mode;
    int     roll_mode;
    int     decay;
    int     filter_constant;
    int     low;
    int     middle;
    int     high;
};

enum SaturationMode {
    NO_SATURATION = 0,
    SATURATE_HIGH_AMPLITUDES = 1
};

enum RollMode {
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
    void on_device_currentIndexChanged(int);
    void on_color_fade_speed_valueChanged(int);
    void on_hue_shift_valueChanged(int);
    void on_bypass_valuesChanged(int,int);
    void on_decay_valueChanged(int);
    void on_avg_size_valueChanged(int);
    void on_avg_mode_currentIndexChanged(int);
    void on_saturation_currentIndexChanged(int);
    void on_roll_mode_currentIndexChanged(int);
    void on_filter_constant_valueChanged(int);
    void on_amplitude_valueChanged(int);
    void on_low_valueChanged(int);
    void on_middle_valueChanged(int);
    void on_high_valueChanged(int);
    void on_defaults_clicked();
    void on_preset_currentIndexChanged(int);

    //void UpdateUiSettings();
    void UpdateGraph(QPixmap);

signals:
    void UpdateGraphSignal(QPixmap) const;

private:    
    Ui::AudioSync *ui;

    /*-----*\
    | Ui    |
    \*-----*/
    QFrame* PrimaryFrame;
    QFrame* AudioSyncFrame;

    /*---------*\
    | Settings  |
    \*---------*/
    AudioSyncSettings current_settings;
    std::vector<AudioSyncSettings> AudioSyncPresets
    {
     AudioSyncSettings {"Default",    50,   0,  0,   256,   8,   0,  0,  0,  80,  100,  100,   100,   100 },
     AudioSyncSettings {"Techno",     80,   0,  0,   256,   8,   1,  0,  0,  30,  100,  40,    100,   160 },
     AudioSyncSettings {"Rock",       50, 165,  0,   256,  12,   1,  0,  0,  70,  100,  120,   140,   140 },
     AudioSyncSettings {"Classical",  60,  67,  0,   256,   8,   0,  0,  0,  98,  100,  80,     80,   175 }
    };

    /*----------*\
    | internals  |
    \*----------*/
    bool                  is_running = false;
    int                   immediate_freq_hue = 0;
    int                   current_freq_hue = 0;
    float                 current_freq_sat = 0;
    float                 current_freq_val = 0;
    std::vector<int>      rainbow_hues;
    std::vector<RGBColor> colors_rotation;    
    int                   audio_device_idx = 0;
    int amplitude = 100;

    RGBColor GetColor(int, int, int, int);

    /*-----*\
    | FFT   |
    \*-----*/
    float         fft[256];
    unsigned char buffer[256];
    float         win_hanning[256];
    float         fft_nrml[256];
    float         fft_fltr[256] = { 0 };

    /*--------*\
    | Methods  |
    \*--------*/
    void Start();
    void Stop();
    void CreateUi();
    void LoadPreset(AudioSyncSettings);
};

#endif // AUDIOSYNC_H
