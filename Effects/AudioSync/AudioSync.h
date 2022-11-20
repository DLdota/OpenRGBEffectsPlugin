#ifndef AUDIOSYNC_H
#define AUDIOSYNC_H

#include "RGBEffect.h"
#include "chuck_fft.h"
#include "hsv.h"
#include "ctkrangeslider.h"
#include "AudioManager.h"
#include "EffectRegisterer.h"
#include "QTooltipedSlider.h"

#include <QLayout>
#include <QComboBox>
#include <QWidget>

#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QTimer>

struct AudioSyncSettings
{
    std::string name;

    float       fade_step;
    int         rainbow_shift;
    int         bypass_min;
    int         bypass_max;
    int         avg_size;
    int         avg_mode;
    int         saturation_mode;
    int         roll_mode;
    int         decay;
    float       filter_constant;
    float       amplitude;
    float       low;
    float       middle;
    float       high;
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

class AudioSync: public RGBEffect
{
    Q_OBJECT
public:
    AudioSync();
    ~AudioSync();

    EFFECT_REGISTERER(ClassName(), CAT_AUDIO, [](){return new AudioSync;});

    static std::string const ClassName() {return "AudioSync";}

    void StepEffect(std::vector<ControllerZone*>)               override;
    void LoadCustomSettings(json)                               override;
    json SaveCustomSettings()                                   override;
    void EffectState(bool)                                      override;

private slots:
    void SelectDeviceChanged(int);
    void FadeStepChanged(int);
    void RainbowShiftChanged(int);
    void BypassChanged(int,int);
    void DecayChanged(int);
    void AvgSizeChanged(int);
    void AvgModeChanged(int);
    void SaturationModeChanged(int);
    void RollModeChanged(int);
    void FilterConstantChanged(int);
    void AmplitudeChanged(int);
    void ToggleAmplitudeChangeInputs();
    void LowChanged(int);
    void MiddleChanged(int);
    void HighChanged(int);
    void RestoreDefaultSettings();
    void UpdateUiSettings();
    void UpdateGraph(QPixmap);
    void PresetChanged(int);

signals:
    void UpdateGraphSignal(QPixmap) const;

private:
    /*-----*\
    | Ui    |
    \*-----*/
    QFrame* PrimaryFrame;
    QFrame* AudioSyncFrame;

    QLabel*         preview;
    ctkRangeSlider* bypass_slider;
    QTooltipedSlider*        rainbow_shift_slider;
    QTooltipedSlider*        fade_step_slider;
    QTooltipedSlider*        decay_slider;
    QTooltipedSlider*        avg_size_slider;
    QComboBox*      avg_mode_selector;
    QComboBox*      saturation_mode_selector;
    QComboBox*      roll_mode_selector;
    QTooltipedSlider*        filter_constant_slider;
    QTooltipedSlider*        amplitude_slider;
    QPushButton*    change_amplitude_button;
    QSpinBox*       amplitude_slider_min;
    QSpinBox*       amplitude_slider_max;
    QTooltipedSlider*        low_slider;
    QTooltipedSlider*        middle_slider;
    QTooltipedSlider*        high_slider;
    QComboBox*      preset_selector;
    QComboBox*      device_list_selector;

    /*---------*\
    | Settings  |
    \*---------*/
    AudioSyncSettings current_settings;
    std::vector<AudioSyncSettings> AudioSyncPresets
    {
     AudioSyncSettings {"Default",    50,   0,  0,   256,   8,   0,  0,  0,  80,   1.0f,   0.5f,   1.0f,   1.0f,   1.0f  },
     AudioSyncSettings {"Techno",     80,   0,  0,   256,   8,   1,  0,  0,  30,   1.0f,   0.75f,  0.4f,   1.0f,   1.6f  },
     AudioSyncSettings {"Rock",       50, 165,  0,   256,  12,   1,  0,  0,  70,   1.0f,   0.9f,   1.2f,   1.4f,   1.4f  },
     AudioSyncSettings {"Classical",  60,  67,  0,   256,   8,   0,  0,  0,  98,   1.0f,   1.0f,   0.8f,   0.8f,   1.73f }
    };

    int amplitude_min_value = 0;
    int amplitude_max_value = 100;

    /*----------*\
    | internals  |
    \*----------*/
    bool                  is_running;
    int                   immediate_freq_hue = 0;
    int                   current_freq_hue = 0;
    float                 current_freq_sat = 0;
    float                 current_freq_val = 0;
    std::vector<int>      rainbow_hues;
    std::vector<RGBColor> colors_rotation;    
    int                   audio_device_idx = 0;

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
    void Init();
    void Start();
    void Stop();
    void CreateUi();
};

#endif // AUDIOSYNC_H
