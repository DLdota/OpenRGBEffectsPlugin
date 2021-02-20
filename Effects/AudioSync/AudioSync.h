#ifndef AUDIOSYNC_H
#define AUDIOSYNC_H

#include "RGBEffect.h"
#include "chuck_fft.h"
#include "hsv.h"
#include "ctkrangeslider.h"
#include "AudioManager.h"

#include <QLayout>
#include <QComboBox>
#include <QWidget>

#include <QSpinBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsView>
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
    int         decay;
    float       filter_constant;
    float       amplitude;
    float       low;
    float       middle;
    float       high;
};

class AudioSync: public QObject, public RGBEffect
{
    Q_OBJECT
public:
    AudioSync();
    ~AudioSync();

    EffectInfo              DefineEffectDetails()                                  override;
    void                    DefineExtraOptions(QLayout*)                           override;
    void                    StepEffect(std::vector<OwnedControllerAndZones>, int)  override;

    void                    SetSpeed(int)                                          override {};
    void                    SetUserColors(std::vector<RGBColor>)                   override {};
    void                    Slider2Changed(int)                                    override {};
    void                    ASelectionWasChanged()                                 override {};
    void                    ToggleRandomColors(bool)                               override {};

    int                     GetSpeed()                                             override {return 0;};
    int                     GetSlider2Val()                                        override {return 0;};
    std::vector<RGBColor>   GetUserColors()                                        override {return {};};

    void                    LoadCustomSettings(json)                               override;
    json                    SaveCustomSettings(json)                               override;

    void                    EffectState(bool)                                      override;

private slots:
    void SelectDeviceChanged(int);
    void FadeStepChanged(int);
    void RainbowShiftChanged(int);
    void BypassChanged(int,int);
    void DecayChanged(int);
    void AvgSizeChanged(int);
    void AvgModeChanged(int);
    void FilterConstantChanged(int);
    void AmplitudeChanged(int);
    void ToggleAmplitudeChangeInputs();
    void LowChanged(int);
    void MiddleChanged(int);
    void HighChanged(int);
    void RestoreDefaultSettings();
    void UpdateUiSettings();
    void UpdateGraph();
    void PresetChanged(int);

signals:
    void UpdateGraphSignal() const;

private:
    EffectInfo EffectDetails;

    /*-----*\
    | Ui    |
    \*-----*/
    QImage*         image;
    QGraphicsScene* scene;
    QPixmap         pixmap;
    QGraphicsView*  graphics_view;
    ctkRangeSlider* bypass_slider;
    QSlider*        rainbow_shift_slider;
    QSlider*        fade_step_slider;
    QSlider*        decay_slider;
    QSlider*        avg_size_slider;
    QComboBox*      avg_mode_selector;
    QSlider*        filter_constant_slider;
    QSlider*        amplitude_slider;
    QPushButton*    change_amplitude_button;
    QSpinBox*       amplitude_slider_min;
    QSpinBox*       amplitude_slider_max;
    QSlider*        low_slider;
    QSlider*        middle_slider;
    QSlider*        high_slider;
    QComboBox*      preset_selector;

    /*---------*\
    | Settings  |
    \*---------*/
    AudioSyncSettings current_settings;
    std::vector<AudioSyncSettings> AudioSyncPresets
    {
     AudioSyncSettings {"Default",    0.5f,   0,     0,   256,   8,   0,   80,   1.0f,   0.5f,   1.0f,   1.0f,   1.0f},
     AudioSyncSettings {"Techno",     0.8f,   0,     0,   256,   8,   1,   30,   1.0f,   0.75f,  0.4f,   1.0f,   1.6f},
     AudioSyncSettings {"Rock",        0.5f, 165,   0,   256,   12,  1,   70,   1.0f,   0.9f,   1.2f,   1.4f,   1.4f},
     AudioSyncSettings {"Classical",  0.6f,  67,    0,   256,   8,   0,   98,   1.0f,   1.0f,   0.8f,   0.8f,   1.73f}
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
    unsigned int          audio_device_idx;
    static const RGBColor OFF = ToRGBColor(0,0,0);

    /*-----*\
    | FFT   |
    \*-----*/
    float         fft[256];
    unsigned char buffer[256];
    float         win_hanning[256];
    float         fft_nrml[256];
    float         fft_fltr[256];

    /*--------*\
    | Methods  |
    \*--------*/
    void Init();
    void Start();
    void Stop();
};

#endif // AUDIOSYNC_H
