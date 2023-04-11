#ifndef AUDIOVISUALIZER_H
#define AUDIOVISUALIZER_H

#include "AudioSignalProcessor.h"
#include "AudioSettings.h"
#include "AudioSettingsStruct.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

#include "ui_AudioVisualizer.h"
#include <QWidget>
#include <vector>

//Special purpose row indices
#define ROW_IDX_BAR_GRAPH           0
#define ROW_IDX_SINGLE_COLOR        1
#define ROW_IDX_SPECTROGRAPH_TOP    2

#define SPECTROGRAPH_COLS           (256)
#define SPECTROGRAPH_END            (SPECTROGRAPH_COLS - 1)
#define SPECTROGRAPH_ROWS           (64 - ROW_IDX_SPECTROGRAPH_TOP)

typedef unsigned int VISUALIZER_PATTERN;
enum
{
    //Static Solid Colors
    VISUALIZER_PATTERN_SOLID_BLACK,
    VISUALIZER_PATTERN_SOLID_WHITE,
    VISUALIZER_PATTERN_SOLID_RED,
    VISUALIZER_PATTERN_SOLID_ORANGE,
    VISUALIZER_PATTERN_SOLID_YELLOW,
    VISUALIZER_PATTERN_SOLID_GREEN,
    VISUALIZER_PATTERN_SOLID_CYAN,
    VISUALIZER_PATTERN_SOLID_BLUE,
    VISUALIZER_PATTERN_SOLID_PURPLE,
    VISUALIZER_PATTERN_SOLID_ELECTRIC_AQUAMARINE,
    //Static Color Patterns    
    VISUALIZER_PATTERN_STATIC_RED_BLUE,
    VISUALIZER_PATTERN_STATIC_CYAN_ORANGE,
    VISUALIZER_PATTERN_STATIC_CYAN_PURPLE,
    VISUALIZER_PATTERN_STATIC_CYAN_ELECTRIC_AQUAMARINE,
    VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED,
    VISUALIZER_PATTERN_STATIC_GREEN_WHITE_RED,
    VISUALIZER_PATTERN_STATIC_BLUE_CYAN_WHITE,
    VISUALIZER_PATTERN_STATIC_RED_WHITE_BLUE,
    VISUALIZER_PATTERN_STATIC_RAINBOW,
    VISUALIZER_PATTERN_STATIC_RAINBOW_INVERSE,
    //Animated Patterns
    VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL,
    VISUALIZER_PATTERN_ANIM_RAINBOW_HSV,
    VISUALIZER_PATTERN_ANIM_COLOR_WHEEL,
    VISUALIZER_PATTERN_ANIM_COLOR_WHEEL_2,
    VISUALIZER_PATTERN_ANIM_SPECTRUM_CYCLE,
    VISUALIZER_PATTERN_ANIM_SINUSOIDAL_CYCLE,

    //Number of Patterns
    VISUALIZER_NUM_PATTERNS
};

const char* const visualizer_pattern_labels[VISUALIZER_NUM_PATTERNS] =
{
    "Black",
    "White",
    "Red",
    "Orange",
    "Yellow",
    "Green",
    "Cyan",
    "Blue",
    "Purple",    
    "Electric Aquamarine",
    "Red/Blue",
    "Cyan/Orange",
    "Cyan/Purple",
    "Cyan/Electric Aquamarine",
    "Green/Yellow/Red",
    "Green/White/Red",
    "Blue/Cyan/White",
    "Red/White/Blue",
    "Rainbow Bars",
    "Rainbow Bars Inverse",
    "Original",
    "Rainbow",
    "Color Wheel",
    "Color Wheel 2",
    "Spectrum Cycle",
    "Sinusoidal Cycle"
};

typedef unsigned int VISUALIZER_SINGLE_COLOR;
enum
{
    //Basic Colors
    VISUALIZER_SINGLE_COLOR_BLACK,
    VISUALIZER_SINGLE_COLOR_WHITE,
    VISUALIZER_SINGLE_COLOR_RED,
    VISUALIZER_SINGLE_COLOR_ORANGE,
    VISUALIZER_SINGLE_COLOR_YELLOW,
    VISUALIZER_SINGLE_COLOR_GREEN,
    VISUALIZER_SINGLE_COLOR_CYAN,
    VISUALIZER_SINGLE_COLOR_BLUE,
    VISUALIZER_SINGLE_COLOR_PURPLE,
    VISUALIZER_SINGLE_COLOR_ELECTRIC_AQUAMARINE,

    VISUALIZER_SINGLE_COLOR_BACKGROUND,
    VISUALIZER_SINGLE_COLOR_FOLLOW_BACKGROUND,
    VISUALIZER_SINGLE_COLOR_FOLLOW_FOREGROUND,

    //Number of Single Color Modes
    VISUALIZER_NUM_SINGLE_COLOR
};

const char* const visualizer_single_color_labels[VISUALIZER_NUM_SINGLE_COLOR] =
{
    "Black",
    "White",
    "Red",
    "Orange",
    "Yellow",
    "Green",
    "Cyan",
    "Blue",
    "Purple",
    "Electric Aquamarine",
    "Background",
    "Follow Background",
    "Follow Foreground"
};

struct vis_pixels
{
    RGBColor pixels[64][256];
};

struct ZoneIndexType
{
    int   x_count;
    int   y_count;
    int*  x_index;
    int*  y_index;
};

namespace Ui {
class AudioVisualizer;
}

class AudioVisualizer: public RGBEffect
{
    Q_OBJECT

public:
    explicit AudioVisualizer(QWidget* parent = nullptr);
    ~AudioVisualizer();

    EFFECT_REGISTERER(ClassName(), CAT_AUDIO, [](){return new AudioVisualizer;});

    static std::string const ClassName() {return "AudioVisualizer";}

    void StepEffect(std::vector<ControllerZone*>) override;
    void EffectState(bool) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private slots:
    /*-------------*\
    | GUI Handling  |
    \*-------------*/
    void Update();
    void OnAudioDeviceChanged(int);

    void on_spinBox_Background_Brightness_valueChanged(int);
    void on_doubleSpinBox_Animation_Speed_valueChanged(double);
    void on_comboBox_Background_Mode_currentIndexChanged(int);
    void on_comboBox_Foreground_Mode_currentIndexChanged(int);
    void on_comboBox_Single_Color_Mode_currentIndexChanged(int);
    void on_checkBox_Reactive_Background_stateChanged(int);
    void on_checkBox_Silent_Background_stateChanged(int);
    void on_doubleSpinBox_Background_Timeout_valueChanged(double);
    void on_audio_settings_clicked();

signals:
    void UpdateGraphSignal();

private:
    /*---------*\
    | GUI bits  |
    \*---------*/
    QPixmap pixmap;
    QImage* image;
    QTimer* timer;

    /*--------------*\
    | State changes  |
    \*--------------*/
    void Start();
    void Stop();

    /*-----------------------*\
    | Pointers                |
    \*-----------------------*/
    Ui::AudioVisualizerUi *ui;

    /*-------------------------------*\
    | Visualizer implementation bits  |
    \*-------------------------------*/
    void LEDUpdateThreadFunction();
    void VisThreadFunction();

    /*--------*\
    | Drawing  |
    \*--------*/
    //void Update();
    void DrawPattern(VISUALIZER_PATTERN pattern, int bright, vis_pixels *pixels);

    vis_pixels pixels_bg;
    vis_pixels pixels_fg;

    vis_pixels pixels_vs1;
    vis_pixels pixels_vs2;

    vis_pixels *pixels_out;
    vis_pixels *pixels_render;

    float anim_speed;
    int bkgd_bright;
    int bkgd_mode;
    bool reactive_bkgd;
    bool silent_bkgd;
    float background_timeout;
    float background_timer;
    int single_color_mode;
    int frgd_mode;

    int     ledstrip_sections_size  = 1;
    int     matrix_setup_pos;
    int     matrix_setup_size;
    bool    ledstrip_mirror_x       = false;
    bool    ledstrip_mirror_y       = false;
    bool    ledstrip_single_color   = false;
    int     ledstrip_rotate_x       = 0;

    std::vector<char *> audio_devices;
    float bkgd_step;
    bool shutdown_flag;

    std::vector<ZoneIndexType*> ZoneMaps;

    AudioSettings                   audio_settings;
    Audio::AudioSettingsStruct      audio_settings_struct;
    AudioSignalProcessor            audio_signal_processor;

    /*------------------*\
    | Drawing Functions  |
    \*------------------*/
    void DrawSolidColor(int bright, RGBColor color, vis_pixels *pixels);
    void DrawSingleColorStatic(float amplitude, RGBColor in_color, vis_pixels *out_pixels);
    void DrawSingleColorForeground(float amplitude, vis_pixels *fg_pixels, vis_pixels *out_pixels);
    void DrawSingleColorBackground(float amplitude, vis_pixels *bg_pixels, vis_pixels *out_pixels);
    void DrawSpectrumCycle(int bright, float bkgd_step, vis_pixels *pixels);
    void DrawSinusoidalCycle(int bright, float bkgd_step, vis_pixels *pixels);
    void DrawRainbow(int bright, float bkgd_step, vis_pixels *pixels);
    void DrawRainbowSinusoidal(int bright, float bkgd_step, vis_pixels *pixels);
    void DrawColorWheel(int bright, float bkgd_step, int center_x, int center_y, vis_pixels *pixels);
    void DrawVerticalBars(int bright, RGBColor * colors, int num_colors, vis_pixels *pixels);
    void DrawHorizontalBars(int bright, RGBColor * colors, int num_colors, vis_pixels *pixels);

    /*-----------*\
    | Zone Setup  |
    \*-----------*/
    void SetupMatrixGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list);
    void SetupLinearGrid(int x_count, int * x_idx_list);
};

#endif // AUDIOVISUALIZER_H
