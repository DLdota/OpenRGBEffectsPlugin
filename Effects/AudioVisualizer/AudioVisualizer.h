#ifndef AUDIOVISUALIZER_H
#define AUDIOVISUALIZER_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "ui_AudioVisualizer.h"
#include <QWidget>
#include <QTimer>
#include <QGraphicsScene>
#include "AudioManager.h"

/*---------------------------------------------------------*\
| C/C++ Standard Library Includes                           |
\*---------------------------------------------------------*/
#include <math.h>
#include <string>
#include <string.h>
#include <vector>
#include <thread>

/*---------------------------------------------------------*\
| Project Includes                                          |
\*---------------------------------------------------------*/
#include "RGBEffect.h"
#include "chuck_fft.h"
#include "AudioManager.h"
#include "hsv.h"

/*---------------------------------------------------------*\
| Audio Library Includes                                    |
\*---------------------------------------------------------*/
#ifdef _WIN32
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#endif

#ifdef __linux__
#include <AL/al.h>
#include <AL/alc.h>
#endif

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif


/*---------------------------------------------------------*\
| Define Windows types for non-Windows builds               |
\*---------------------------------------------------------*/
#ifndef _WIN32
#include <unistd.h>

typedef unsigned int    COLORREF;
typedef unsigned char   BYTE;
typedef bool            boolean;
#define RGB(r, g, b)    (((unsigned char)r) | ((unsigned char)g << 8) | ((unsigned char)b << 16))
#define GetRValue(rgb)  ((unsigned char) (rgb))
#define GetGValue(rgb)  ((unsigned char) ((rgb) >> 8))
#define GetBValue(rgb)  ((unsigned char) ((rgb) >> 16))
#define Sleep(ms)       (usleep(ms * 1000))
#define LPSTR           char *
#define strtok_s        strtok_r
#endif

#define RGB2BGR(a_ulColor) (a_ulColor & 0xFF000000) | ((a_ulColor & 0xFF0000) >> 16) | (a_ulColor & 0x00FF00) | ((a_ulColor & 0x0000FF) << 16)

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
    //Static Color Patterns    
    VISUALIZER_PATTERN_STATIC_RED_BLUE,
    VISUALIZER_PATTERN_STATIC_CYAN_ORANGE,
    VISUALIZER_PATTERN_STATIC_CYAN_PURPLE,
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
    "Red/Blue",
    "Cyan/Orange",
    "Cyan/Purple",
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
    int                 x_count;
    int                 y_count;
    int *               x_index;
    int *               y_index;
};

namespace Ui {
class AudioVisualizer;
}

class AudioVisualizer:  public QWidget, public RGBEffect
{
    Q_OBJECT

public:
    explicit AudioVisualizer(QWidget* parent = nullptr);
    ~AudioVisualizer();

    EFFECT_REGISTERER(ClassName(), [](){return new AudioVisualizer;});

    static std::string const ClassName() {return "AudioVisualizer";}

    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone>) override;
    void EffectState(bool) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

private slots:
    /*-------------*\
    | GUI Handling  |
    \*-------------*/
    void update();
    void on_lineEdit_Background_Brightness_textChanged(const QString &);
    void on_lineEdit_Animation_Speed_textChanged(const QString &);

    /*---------------------*\
    | Amp, Size, and Decay  |
    \*---------------------*/
    void on_lineEdit_Amplitude_textChanged(const QString &);
    void on_lineEdit_Average_Size_textChanged(const QString &);
    void on_lineEdit_Decay_textChanged(const QString &);

    /*-------------*\
    | Normalization |
    \*-------------*/
    void on_lineEdit_Normalization_Offset_textChanged(const QString &);
    void on_lineEdit_Normalization_Scale_textChanged(const QString &);

    /*------------------------------*\
    | Color and Brightness Settings  |
    \*------------------------------*/
    void on_comboBox_FFT_Window_Mode_currentIndexChanged(int);
    void on_comboBox_Background_Mode_currentIndexChanged(int);
    void on_comboBox_Foreground_Mode_currentIndexChanged(int);
    void on_comboBox_Single_Color_Mode_currentIndexChanged(int);
    void on_comboBox_Average_Mode_currentIndexChanged(int);
    void on_checkBox_Reactive_Background_clicked(bool);
    void on_comboBox_Audio_Device_currentIndexChanged(int);
    void on_lineEdit_Filter_Constant_textChanged(const QString &);
    void on_checkBox_Silent_Background_clicked(bool);
    void on_lineEdit_Background_Timeout_textChanged(const QString &);

    void on_ShowHideSettings_clicked();

private:
    /*---------*\
    | GUI bits  |
    \*---------*/
    QGraphicsScene* scene;
    QPixmap pixmap;
    QImage* image;
    QTimer* timer;

    /*--------------*\
    | State changes  |
    \*--------------*/
    bool Hiding = false;
    bool RegisteredForDevice = false;
    bool EffectActive = false;
    void SetDevice();


    /*-----------------------*\
    | Pointers                |
    \*-----------------------*/
    Ui::AudioVisualizerUi *ui;

    /*-------------------------------*\
    | Visualizer implementation bits  |
    \*-------------------------------*/
    void LEDUpdateThreadFunction();
    void VisThreadFunction();
    void SetNormalization(float offset, float scale);

    /*--------*\
    | Drawing  |
    \*--------*/
    void Update();
    void DrawPattern(VISUALIZER_PATTERN pattern, int bright, vis_pixels *pixels);

    /*-----------------------*\
    | Various Peices of Data  |
    |                         |
    | fft array               |
    |                         |
    | Amplitude               |
    | Average Mode            |
    | Average Size            |
    | Window Mode             |
    | Decay Speed             |
    | Audio Device ID         |
    |                         |
    | Update UI flag          |
    |                         |
    | Background Color Array  |
    | Foreground Color Array  |
    |                         |
    | Image 1                 |
    | Image 2                 |
    |                         |
    | Output Pointer          |
    | Render Pointer          |
    |                         |
    | Animation Speed         |
    | Background Brightness   |
    | Background Mode         |
    | Reactive Background     |
    | Silent Background       |
    | Background Timeout      |
    | Background Timer        |
    |                         |
    | Single Color Mode       |
    |                         |
    | Normalization Offset    |
    | Normalization Scaling   |
    |                         |
    | Filter Constant         |
    |                         |
    | Foreground Mode         |
    |                         |
    | Audio Device List       |
    |------More Misc----------|
    | Background Step         |
    |                         |
    | FFT Variables           |
    |                         |
    | Settings Changed Flag   |
    | Shutdown Flag           |
    |                         |
    | Audio Sample Buffer     |
    |                         |
    | List of Zone Maps       |
    |----Drawing Functions----|
    | Solid Color             |
    | Solid Color Static      |
    | Solid Color Foreground  |
    | Solid Color Background  |
    | Spectrum Cycling        |
    | Sine Spectrum Cycling   |
    | Rainbow Wave            |
    | Sine Rainbow Wave       |
    | Color Wheel             |
    | Vertical Bars           |
    | Horizontal Bars         |
    |-------Zone setup--------|
    | SetupMatrixGrid         |
    | SetupLinearGrid         |
    \*-----------------------*/
    float fft[256];

    int amplitude;
    int avg_mode;
    int avg_size;
    int window_mode;
    int decay;
    unsigned int audio_device_idx = 0;
    unsigned int previous_audio_device_idx = 0;

    bool update_ui;

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

    float nrml_ofst;
    float nrml_scl;

    float filter_constant;

    int frgd_mode;

    std::vector<char *> audio_devices;

    float bkgd_step;

    float win_hanning[256];
    float win_hamming[256];
    float win_blackman[256];
    float fft_tmp[512];

    bool shutdown_flag;

    unsigned char buffer[256];

    std::vector<ZoneIndexType*> ZoneMaps;

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
