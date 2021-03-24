/*---------------------------------------------------------*\
|  Definitions for Keyboard Visualizer                      |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef VISUALIZER_H
#define VISUALIZER_H

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
#include "VisualizerDefines.h"
#include "RGBEffect.h"
#include "chuck_fft.h"
#include "AudioManager.h"
#include "hsv.h"
#include "net_port.h"

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

struct ZoneIndexType
{
    int                 x_count;
    int                 y_count;
    int *               x_index;
    int *               y_index;
};

class Visualizer
{
public:

    /*--------*\
    | Setup    |
    \*--------*/
    Visualizer() {};
    void Initialize();

    /*----------*\
    | Updating   |
    \*----------*/
    void StepFrame(std::vector<OwnedControllerAndZones> Controllers, int FPS);
    void LEDUpdateThreadFunction();
    void VisThreadFunction();
    void SetNormalization(float offset, float scale);

    /*--------*\
    | Settings |
    \*--------*/
    void OnSettingsChanged();
    json SaveSettings();
    void LoadSettings(json Settings);


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
    |-------Windows-----------|
    | Device Enumerator       |
    | pMMDDevice List         |
    | IsCapture bool vector   |
    | Device Collection       |
    | Audio Client            |
    | Audio Capture Client    |
    | Wave Format             |
    |                         |
    |-------Linux-------------|
    | Audio Device Pointer    |
    |                         |
    |------More Misc----------|
    | FPS int                 |
    |                         |
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
private:
    float FPS;

    float bkgd_step;

    float win_hanning[256];
    float win_hamming[256];
    float win_blackman[256];
    float fft_tmp[512];

    bool settings_changed;
    bool shutdown_flag;

    unsigned char buffer[256];

    std::vector<std::vector<ZoneIndexType*>> ZoneMaps;

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
};

#endif
