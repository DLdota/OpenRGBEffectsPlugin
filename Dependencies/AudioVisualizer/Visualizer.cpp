/*---------------------------------------------------------*\
|  Processing Code for Keyboard Visualizer                  |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
|  Modded by CoffeeIsLife (gitlab.com/herosilas12)          |
\*---------------------------------------------------------*/
#include "Visualizer.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*---------------------------------------------------------*\
| Global variables                                          |
\*---------------------------------------------------------*/
char *  net_string;
int     ledstrip_sections_size  = 1;
int     matrix_setup_pos;
int     matrix_setup_size;
float   fft_nrml[256];
float   fft_fltr[256];
bool    ledstrip_mirror_x       = false;
bool    ledstrip_mirror_y       = false;
bool    ledstrip_single_color   = false;
int     ledstrip_rotate_x       = 0;

/*---------------------------------------------------------*\
| Visualizer class implementation                           |
\*---------------------------------------------------------*/

void Visualizer::Initialize()
{
    /*---------------------*\
    | Setup default values  |
    \*---------------------*/
    background_timer     = 0;
    background_timeout   = 120;

    amplitude            = 100;
    anim_speed           = 100.0f;
    avg_mode             = 0;
    avg_size             = 8;
    bkgd_step            = 0;
    bkgd_bright          = 100;
    bkgd_mode            = VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL;
    window_mode          = 1;
    decay                = 80;
    frgd_mode            = VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED;
    single_color_mode    = VISUALIZER_SINGLE_COLOR_FOLLOW_FOREGROUND;
    reactive_bkgd        = false;
    audio_device_idx     = 0;
    filter_constant      = 1.0f;

    settings_changed     = false;
    update_ui            = false;
    shutdown_flag        = false;

    hanning(win_hanning, 256);
    hamming(win_hamming, 256);
    blackman(win_blackman, 256);

    nrml_ofst            = 0.04f;
    nrml_scl             = 0.5f;

    pixels_render = &pixels_vs1;
    pixels_out = &pixels_vs2;

    SetNormalization(nrml_ofst, nrml_scl);
}

void Visualizer::LoadSettings(json Settings)
{
    /*----------------------------------------------*\
    | Set all values to the settings stored in json  |
    \*----------------------------------------------*/
    if (Settings.contains("SilentBackGround"))     silent_bkgd        = Settings["SilentBackGround"];
    if (Settings.contains("Amplitude"))            amplitude          = Settings["Amplitude"];
    if (Settings.contains("BackgroundBrightness")) bkgd_bright        = Settings["BackgroundBrightness"];
    if (Settings.contains("AverageSize"))          avg_size           = Settings["AverageSize"];
    if (Settings.contains("Decay"))                decay              = Settings["Decay"];
    if (Settings.contains("NormalizationOffset"))  nrml_ofst          = Settings["NormalizationOffset"];
    if (Settings.contains("NormalizationScale"))   nrml_scl           = Settings["NormalizationScale"];
    if (Settings.contains("FilterConstant"))       filter_constant    = Settings["FilterConstant"];
    if (Settings.contains("BackgroundMode"))       bkgd_mode          = Settings["BackgroundMode"];
    if (Settings.contains("ForegroundMode"))       frgd_mode          = Settings["ForegroundMode"];
    if (Settings.contains("SingleColorMode"))      single_color_mode  = Settings["SingleColorMode"];
    if (Settings.contains("AverageMode"))          avg_mode           = Settings["AverageMode"];
    if (Settings.contains("AnimationSpeed"))       anim_speed         = Settings["AnimationSpeed"];
    if (Settings.contains("ReactiveBackground"))   reactive_bkgd      = Settings["ReactiveBackground"];
    if (Settings.contains("SilentBackground"))     silent_bkgd        = Settings["SilentBackground"];
    if (Settings.contains("BackgroundTimeout"))    background_timeout = Settings["BackgroundTimeout"];
    if (Settings.contains("AudioDevice"))          audio_device_idx   = Settings["AudioDevice"];

    /*
    silent_bkgd        = Settings["SilentBackGround"];
    amplitude          = Settings["Amplitude"];
    bkgd_bright        = Settings["BackgroundBrightness"];
    avg_size           = Settings["AverageSize"];
    decay              = Settings["Decay"];
    nrml_ofst          = Settings["NormalizationOffset"];
    nrml_scl           = Settings["NormalizationScale"];
    filter_constant    = Settings["FilterConstant"];
    bkgd_mode          = Settings["BackgroundMode"];
    frgd_mode          = Settings["ForegroundMode"];
    single_color_mode  = Settings["SingleColorMode"];
    avg_mode           = Settings["AverageMode"];
    anim_speed         = Settings["AnimationSpeed"];
    reactive_bkgd      = Settings["ReactiveBackground"];
    silent_bkgd        = Settings["SilentBackground"];
    background_timeout = Settings["BackgroundTimeout"];
    audio_device_idx   = Settings["AudioDevice"];
    */
}

json Visualizer::SaveSettings()
{
    json Settings;
    /*---------------------------------------------*\
    | Only allow for Silent OR Reactive. Not both   |
    \*---------------------------------------------*/
    if ((silent_bkgd == TRUE) && (reactive_bkgd == TRUE))
    {
        silent_bkgd = FALSE;
    }

    Settings["SilentBackGround"] = silent_bkgd;
    Settings["Amplitude"] = amplitude;
    Settings["BackgroundBrightness"] = bkgd_bright;
    Settings["AverageSize"] = avg_size;
    Settings["Decay"] = decay;
    Settings["NormalizationOffset"] = nrml_ofst;
    Settings["NormalizationScale"] = nrml_scl;
    Settings["FilterConstant"] = filter_constant;
    Settings["BackgroundMode"] = bkgd_mode;
    Settings["ForegroundMode"] = frgd_mode;
    Settings["SingleColorMode"] = single_color_mode;
    Settings["AverageMode"] = avg_mode;
    Settings["AnimationSpeed"] = anim_speed;
    Settings["ReactiveBackground"] = reactive_bkgd;
    Settings["SilentBackground"] = silent_bkgd;
    Settings["BackgroundTimeout"] = background_timeout;
    Settings["AudioDevice"] = audio_device_idx;

    return Settings;
}

void Visualizer::SetNormalization(float offset, float scale)
{
    for (int i = 0; i < 256; i++)
    {
        fft[i] = 0.0f;
        fft_nrml[i] = offset + (scale * (i / 256.0f));
    }
}

void Visualizer::OnSettingsChanged()
{
    settings_changed = true;
}

void Visualizer::Update()
{
    float fft_tmp[512];
    for (int i = 0; i < 256; i++)
    {
        /*------------------*\
        | Clear the buffers  |
        \*------------------*/
        fft_tmp[i] = 0;

        /*-----------------------------------------------------------------------------------------*\
        | fft = fft * ( a decimal of decay divided by the opposite of FPS (1 for 60 and 60 for 1)   |
        \*-----------------------------------------------------------------------------------------*/
        fft[i] = fft[i] * ((float(decay) / 100.0f) / (60 / this->FPS) );
    }

    AudioManager::get()->Capture(audio_device_idx, fft_tmp);

    #ifdef _WIN32
    for (int i = 0; i < 512; i++)
    {
        fft_tmp[i] *= amplitude;
    }
    #else
    for (int i = 0; i < 512; i++)
    {
        fft_tmp[i] = (fft_tmp[i] - 128.0f) * (amplitude / 128.0f);
    }
    #endif

    /*----------------------*\
    | Apply selected window  |
    \*----------------------*/
    switch (window_mode)
    {
    case 0:
        break;

    case 1:
        apply_window(fft_tmp, win_hanning, 256);
        break;

    case 2:
        apply_window(fft_tmp, win_hamming, 256);
        break;

    case 3:
        apply_window(fft_tmp, win_blackman, 256);
        break;

    default:
        break;
    }

    /*------------------------*\
    | Run the FFT calculation  |
    \*------------------------*/
    rfft(fft_tmp, 256, 1);

    fft_tmp[0] = fft_tmp[2];

    apply_window(fft_tmp, fft_nrml, 256);

    /*----------------------*\
    | Compute FFT magnitude  |
    \*----------------------*/
    for (int i = 0; i < 128; i += 2)
    {
        float fftmag;

        /*---------------------------------------------------------------------------------*\
        | Compute magnitude from real and imaginary components of FFT and apply simple LPF  |
        \*---------------------------------------------------------------------------------*/
        fftmag = (float)sqrt((fft_tmp[i] * fft_tmp[i]) + (fft_tmp[i + 1] * fft_tmp[i + 1]));

        /*----------------------------------------------------------------------------------------*\
        | Apply a slight logarithmic filter to minimize noise from very low amplitude frequencies  |
        \*----------------------------------------------------------------------------------------*/
        fftmag = ( 0.5f * log10(1.1f * fftmag) ) + ( 0.9f * fftmag );

        /*---------------------------*\
        | Limit FFT magnitude to 1.0  |
        \*---------------------------*/
        if (fftmag > 1.0f)
        {
            fftmag = 1.0f;
        }

        /*----------------------------------------------------------*\
        | Update to new values only if greater than previous values  |
        \*----------------------------------------------------------*/
        if (fftmag > fft[i*2])
        {
            fft[i*2] = fftmag;;
        }

        /*----------------------------*\
        | Prevent from going negative  |
        \*----------------------------*/
        if (fft[i*2] < 0.0f)
        {
            fft[i*2] = 0.0f;
        }

        /*----------------------------------------------------------------*\
        | Set odd indexes to match their corresponding even index, as the  |
        | FFT input array uses two indices for one value (real+imaginary)  |
        \*----------------------------------------------------------------*/
        fft[(i * 2) + 1] = fft[i * 2];
        fft[(i * 2) + 2] = fft[i * 2];
        fft[(i * 2) + 3] = fft[i * 2];
    }

    if (avg_mode == 0)
    {
        /*--------------------------------------------*\
        | Apply averaging over given number of values  |
        \*--------------------------------------------*/
        int k;
        float sum1 = 0;
        float sum2 = 0;
        for (k = 0; k < avg_size; k++)
        {
            sum1 += fft[k];
            sum2 += fft[255 - k];
        }
        /*------------------------------*\
        | Compute averages for end bars  |
        \*------------------------------*/
        sum1 = sum1 / k;
        sum2 = sum2 / k;
        for (k = 0; k < avg_size; k++)
        {
            fft[k] = sum1;
            fft[255 - k] = sum2;
        }
        for (int i = 0; i < (256 - avg_size); i += avg_size)
        {
            float sum = 0;
            for (int j = 0; j < avg_size; j += 1)
            {
                sum += fft[i + j];
            }

            float avg = sum / avg_size;

            for (int j = 0; j < avg_size; j += 1)
            {
                fft[i + j] = avg;
            }
        }
    }
    else if(avg_mode == 1)
    {
        for (int i = 0; i < avg_size; i++)
        {
            float sum1 = 0;
            float sum2 = 0;
            int j;
            for (j = 0; j <= i + avg_size; j++)
            {
                sum1 += fft[j];
                sum2 += fft[255 - j];
            }
            fft[i] = sum1 / j;
            fft[255 - i] = sum2 / j;
        }
        for (int i = avg_size; i < 256 - avg_size; i++)
        {
            float sum = 0;
            for (int j = 1; j <= avg_size; j++)
            {
                sum += fft[i - j];
                sum += fft[i + j];
            }
            sum += fft[i];

            fft[i] = sum / (2 * avg_size + 1);
        }
    }
    for(int i = 0; i < 256; i++)
    {
        fft_fltr[i] = fft_fltr[i] + (filter_constant * (fft[i] - fft_fltr[i]));
    }
}

void Visualizer::DrawSolidColor(int bright, RGBColor color, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = RGB(((bright * GetRValue(color)) / 256), ((bright * GetGValue(color)) / 256), ((bright * GetBValue(color)) / 256));
        }
    }
}

void Visualizer::DrawSingleColorStatic(float amplitude, RGBColor in_color, vis_pixels *out_pixels)
{
    if (amplitude >= 1.0f)
    {
        amplitude = 1.0f;
    }
    else if (amplitude <= 0.0f)
    {
        amplitude = 0.0f;
    }

    int out_color = RGB(((amplitude * GetRValue(in_color))), ((amplitude * GetGValue(in_color))), ((amplitude * GetBValue(in_color))));
    for (int x = 0; x < 256; x++)
    {
        out_pixels->pixels[ROW_IDX_SINGLE_COLOR][x] = out_color;
    }
}

void Visualizer::DrawSingleColorForeground(float amplitude, vis_pixels *fg_pixels, vis_pixels *out_pixels)
{
    if (amplitude >= 1.0f)
    {
        amplitude = 1.0f;
    }
    else if (amplitude <= 0.0f)
    {
        amplitude = 0.0f;
    }

    int idx = (int)(64.0f - (amplitude * 62.0f));
    int in_color = fg_pixels->pixels[idx][0];
    int out_color = RGB(((amplitude * GetRValue(in_color))), ((amplitude * GetGValue(in_color))), ((amplitude * GetBValue(in_color))));
    for (int x = 0; x < 256; x++)
    {
        if (frgd_mode >= VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL)
        {
            in_color = fg_pixels->pixels[ROW_IDX_SINGLE_COLOR][x];
            out_color = RGB(((amplitude * GetRValue(in_color))), ((amplitude * GetGValue(in_color))), ((amplitude * GetBValue(in_color))));
        }

        out_pixels->pixels[ROW_IDX_SINGLE_COLOR][x] = out_color;
    }
}

void Visualizer::DrawSingleColorBackground(float amplitude, vis_pixels *bg_pixels, vis_pixels *out_pixels)
{
    if (amplitude >= 1.0f)
    {
        amplitude = 1.0f;
    }
    else if (amplitude <= 0.0f)
    {
        amplitude = 0.0f;
    }

    for (int x = 0; x < 256; x++)
    {
        int in_color = bg_pixels->pixels[ROW_IDX_SINGLE_COLOR][x];
        int out_color = RGB(((amplitude * GetRValue(in_color))), ((amplitude * GetGValue(in_color))), ((amplitude * GetBValue(in_color))));
        out_pixels->pixels[ROW_IDX_SINGLE_COLOR][x] = out_color;
    }
}

void Visualizer::DrawSpectrumCycle(int bright, float bkgd_step, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    hsv_t hsv2 = { 0, 0, 0 };
    hsv2.hue = (int)bkgd_step;
    hsv2.saturation = 255;
    hsv2.value = (unsigned char)bright;
    RGBColor color = hsv2rgb(&hsv2);

    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = color;
        }
    }
}

void Visualizer::DrawSinusoidalCycle(int bright, float bkgd_step, vis_pixels *pixels)
{
    RGBColor color;
    bright = (int)(bright * (255.0f / 100.0f));
    int red = (int)(127 * (sin(((((int)(((360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f)) + 1));
    int grn = (int)(127 * (sin(((((int)(((360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) - (6.28f / 3)) + 1));
    int blu = (int)(127 * (sin(((((int)(((360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) + (6.28f / 3)) + 1));
    color = RGB(((bright * red) / 256), ((bright * grn) / 256), ((bright * blu) / 256));

    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = color;
        }
    }
}

void Visualizer::DrawRainbow(int bright, float bkgd_step, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int hsv_h = ((int)(bkgd_step + (256 - x)) % 360);
            hsv_t hsv = { 0, 0, 0 };
            hsv.hue = hsv_h;
            hsv.saturation = 255;
            hsv.value = (unsigned char)bright;
            pixels->pixels[y][x] = hsv2rgb(&hsv);
        }
    }
}

void Visualizer::DrawRainbowSinusoidal(int bright, float bkgd_step, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int red = (int)(127 * (sin(((((int)((x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f)) + 1));
            int grn = (int)(127 * (sin(((((int)((x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) - (6.28f / 3)) + 1));
            int blu = (int)(127 * (sin(((((int)((x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) + (6.28f / 3)) + 1));
            pixels->pixels[y][x] = RGB(((bright * red) / 256), ((bright * grn) / 256), ((bright * blu) / 256));
        }
    }
}

void Visualizer::DrawColorWheel(int bright, float bkgd_step, int center_x, int center_y, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            float hue = (float)(bkgd_step + (int)(180 + atan2(y - center_y, x - center_x) * (180.0 / 3.14159)) % 360);
            hsv_t hsv2 = { 0, 0, 0 };
            hsv2.hue = (int)hue;
            hsv2.saturation = 255;
            hsv2.value = (unsigned char)bright;
            pixels->pixels[y][x] = hsv2rgb(&hsv2);
        }
    }
}

void Visualizer::DrawVerticalBars(int bright, RGBColor * colors, int num_colors, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int idx = (int)((float)x * ((float)num_colors / 255.0f));
            pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
        }
    }
}

void Visualizer::DrawHorizontalBars(int bright, RGBColor * colors, int num_colors, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            if (y == ROW_IDX_BAR_GRAPH)
            {
                if (x < 128)
                {
                    int idx = (int)(num_colors - ((float)x * ((float)num_colors / 128.0f)));
                    if (idx >= num_colors)
                    {
                        idx = num_colors - 1;
                    }
                    pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
                }
                else
                {
                    int idx = (int)(((float)(x - 128) * ((float)num_colors / 128.0f)));
                    pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
                }
            }
            else
            {
                int idx = (int)(num_colors - ((float)y * ((float)num_colors / 63.0f)));
                pixels->pixels[y][x] = RGB(((bright * GetRValue(colors[idx])) / 256), ((bright * GetGValue(colors[idx])) / 256), ((bright * GetBValue(colors[idx])) / 256));
            }
        }
    }
}

void Visualizer::DrawPattern(VISUALIZER_PATTERN pattern, int bright, vis_pixels *pixels)
{
    switch (pattern)
    {
    case VISUALIZER_PATTERN_SOLID_BLACK:
        DrawSolidColor(bright, 0x00000000, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_WHITE:
        DrawSolidColor(bright, 0x00FFFFFF, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_RED:
        DrawSolidColor(bright, 0x000000FF, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_ORANGE:
        DrawSolidColor(bright, 0x000040FF, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_YELLOW:
        DrawSolidColor(bright, 0x0000FFFF, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_GREEN:
        DrawSolidColor(bright, 0x0000FF00, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_CYAN:
        DrawSolidColor(bright, 0x00FFFF00, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_BLUE:
        DrawSolidColor(bright, 0x00FF0000, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_PURPLE:
        DrawSolidColor(bright, 0x00FF0040, pixels);
        break;

    case VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED:
        {
        RGBColor colors[] = { 0x0000FF00, 0x0000FFFF, 0x000000FF };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_GREEN_WHITE_RED:
        {
        RGBColor colors[] = { 0x0000FF00, 0x00FFFFFF, 0x000000FF };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_BLUE_CYAN_WHITE:
        {
        RGBColor colors[] = { 0x00FF0000, 0x00FFFF00, 0x00FFFFFF };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_RED_WHITE_BLUE:
        {
        RGBColor colors[] = { 0x000000FF, 0x00FFFFFF, 0x00FF0000 };
        DrawHorizontalBars(bright, colors, 3, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_RAINBOW:
        {
        RGBColor colors[] = { 0x000000FF, 0x0000FFFF, 0x0000FF00, 0x00FFFF00, 0x00FF0000, 0x00FF00FF };
        DrawHorizontalBars(bright, colors, 6, pixels);
        }
        break;

    case VISUALIZER_PATTERN_STATIC_RAINBOW_INVERSE:
        {
        RGBColor colors[] = { 0x00FF00FF, 0x00FF0000, 0x00FFFF00, 0x0000FF00, 0x0000FFFF, 0x000000FF };
        DrawHorizontalBars(bright, colors, 6, pixels);
        }
        break;

    case VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL:
        DrawRainbowSinusoidal(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_RAINBOW_HSV:
        DrawRainbow(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_COLOR_WHEEL:
        DrawColorWheel(bright, bkgd_step, 128, 32, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_COLOR_WHEEL_2:
        DrawColorWheel(bright, bkgd_step, 128, 64, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_SPECTRUM_CYCLE:
        DrawSpectrumCycle(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_SINUSOIDAL_CYCLE:
        DrawSinusoidalCycle(bright, bkgd_step, pixels);
        break;
    }
}

static void SetupMatrixGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list)
{
    for(int x = 0; x < x_count; x++)
    {
        if(x_count < 10)
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count))) + (0.5f * (SPECTROGRAPH_COLS / (x_count))));
        }
        else if(x < ((x_count) / 2))
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) + (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }
        else
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (x_count - 1))) - (0.5f * (SPECTROGRAPH_COLS / (x_count - 1))));
        }

    }
    for(int y = 0; y < y_count; y++)
    {
        y_idx_list[y] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (y * (SPECTROGRAPH_ROWS / y_count)) + (0.5f * (SPECTROGRAPH_ROWS / y_count)));
    }
}

static void SetupLinearGrid(int x_count, int * x_idx_list)
{
    if((x_count % 2) == 0)
    {
        /*--------------------*\
        | Even number of LEDs  |
        \*--------------------*/
        for(int x = 0; x < x_count; x++)
        {
            x_idx_list[x] = (int)((float)x * (256.0f / (float)x_count)) + (128.0f / (float)x_count);
        }
    }
    else
    {
        /*-------------------*\
        | Odd number of LEDs  |
        \*-------------------*/
        for(int x = 0; x < x_count; x++)
        {
            if (x == (x_count / 2))
            {
                x_idx_list[x] = 128;
            }
            else if (x < ((x_count / 2) + 1))
            {
                x_idx_list[x] = (x_count / 2) + ((x + 1) * (256 / (x_count + 1)));
            }
            else
            {
                x_idx_list[x] = ((x_count / 2) + 1) + (x * (256 / (x_count + 1)));
            }
        }
    }
}

void Visualizer::StepFrame(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    this->FPS = FPS;
    Update();

    /*-------------------------*\
    | Overflow background step  |
    \*-------------------------*/
    if (bkgd_step >= 360.0f) bkgd_step = 0.0f;
    if (bkgd_step < 0.0f) bkgd_step = 360.0f;

    /*-----------------------*\
    | Draw active background  |
    \*-----------------------*/
    DrawPattern(bkgd_mode, bkgd_bright, &pixels_bg);

    /*------------------------*\
    | Draw active foreground   |
    \*------------------------*/
    DrawPattern(frgd_mode, 100, &pixels_fg);

    float brightness = fft_fltr[5];

    /*----------------------------------------------------------------------*\
    | If music isn't playing, fade in the single color LEDs after 2 seconds  |
    \*----------------------------------------------------------------------*/
    background_timer = background_timer + (1 / this->FPS);

    if (shutdown_flag == true)
    {
        if (background_timer >= background_timeout)
        {
            brightness = 0.0f;
        }
        else
        {
            brightness = ((background_timeout - background_timer) / (1.0f * background_timeout));
        }
    }
    else if (background_timeout > 0)
    {
        for (int i = 0; i < 128; i++)
        {
            if (fft_fltr[2 * i] >= 0.0001f)
            {
                background_timer = 0;
            }
        }
        if (background_timer >= background_timeout)
        {
            if (background_timer >= (3 * background_timeout))
            {
                background_timer = (3 * background_timeout);
            }
            brightness = (background_timer - background_timeout) / (2.0f * background_timeout);
        }
    }

    /*------------------------------------------------------*\
    | Loop through all 256x64 pixels in visualization image  |
    \*------------------------------------------------------*/
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            /*-----------------------------*\
            | Draw Spectrograph Foreground  |
            \*-----------------------------*/
            if (fft_fltr[x] >((1 / 64.0f)*(64.0f - y)))
            {
                if (shutdown_flag == true)
                {
                    int in_color = pixels_fg.pixels[y][x];
                    pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                }
                else
                {
                    pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                }
            }
            else
            {
                if(reactive_bkgd || silent_bkgd)
                {
                    if (!silent_bkgd || ((background_timer >= background_timeout) && (background_timeout > 0)))
                    {
                        int in_color = pixels_bg.pixels[y][x];
                        pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                    }
                    else
                    {
                        pixels_render->pixels[y][x] = RGB(0, 0, 0);
                    }
                }
                else
                {
                    pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                }

            }

            /*--------------------------*\
            | Draw Bar Graph Foreground  |
            \*--------------------------*/
            if (y == ROW_IDX_BAR_GRAPH)
            {
                if (x < 128)
                {
                    if ((fft_fltr[5] - 0.05f) >((1 / 128.0f)*(127-x)))
                    {
                        if (shutdown_flag == true)
                        {
                            int in_color = pixels_fg.pixels[y][x];
                            pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                        }
                    }
                    else
                    {
                        if (reactive_bkgd || silent_bkgd)
                        {
                            if (!silent_bkgd || ((background_timer >= background_timeout) && (background_timeout > 0)))
                            {
                                int in_color = pixels_bg.pixels[y][x];
                                pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                            }
                            else
                            {
                                pixels_render->pixels[y][x] = RGB(0, 0, 0);
                            }
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                        }
                    }
                }
                else
                {
                    if ((fft_fltr[5] - 0.05f) >((1 / 128.0f)*((x-128))))
                    {
                        if (shutdown_flag == true)
                        {
                            int in_color = pixels_fg.pixels[y][x];
                            pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                        }
                    }
                    else
                    {
                        if (reactive_bkgd || silent_bkgd)
                        {
                            if (!silent_bkgd || (background_timer >= background_timeout))
                            {
                                int in_color = pixels_bg.pixels[y][x];
                                pixels_render->pixels[y][x] = RGB(((brightness * GetRValue(in_color))), ((brightness * GetGValue(in_color))), ((brightness * GetBValue(in_color))));
                            }
                            else
                            {
                                pixels_render->pixels[y][x] = RGB(0, 0, 0);
                            }
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                        }
                    }
                }
            }
        }
    }

    if (single_color_mode == VISUALIZER_SINGLE_COLOR_FOLLOW_BACKGROUND)
    {
        brightness = (bkgd_bright / 100.0f) * brightness;
    }

    if ((background_timeout <= 0 ) || (background_timer < background_timeout))
    {
        /*--------------------------------------------------------*\
        | Draw brightness based visualizer for single LED devices  |
        \*--------------------------------------------------------*/
        switch (single_color_mode)
        {
        case VISUALIZER_SINGLE_COLOR_BLACK:
            DrawSingleColorStatic(brightness, 0x00000000, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_WHITE:
            DrawSingleColorStatic(brightness, 0x00FFFFFF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_RED:
            DrawSingleColorStatic(brightness, 0x000000FF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_ORANGE:
            DrawSingleColorStatic(brightness, 0x000080FF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_YELLOW:
            DrawSingleColorStatic(brightness, 0x0000FFFF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_GREEN:
            DrawSingleColorStatic(brightness, 0x0000FF00, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_CYAN:
            DrawSingleColorStatic(brightness, 0x00FFFF00, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_BLUE:
            DrawSingleColorStatic(brightness, 0x00FF0000, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_PURPLE:
            DrawSingleColorStatic(brightness, 0x00FF00FF, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_BACKGROUND:
            /*----------------------------------------------------------*\
            | Intentionally do nothing, leave the background unmodified  |
            \*----------------------------------------------------------*/
            break;

        case VISUALIZER_SINGLE_COLOR_FOLLOW_BACKGROUND:
            DrawSingleColorBackground(brightness, &pixels_bg, pixels_render);
            break;

        case VISUALIZER_SINGLE_COLOR_FOLLOW_FOREGROUND:
            DrawSingleColorForeground(brightness, &pixels_fg, pixels_render);
            break;
        }
    }

    /*-------------*\
    | Swap buffers  |
    \*-------------*/
    if (pixels_render == &pixels_vs1)
    {
        pixels_render = &pixels_vs2;
        pixels_out = &pixels_vs1;
    }
    else
    {
        pixels_render = &pixels_vs1;
        pixels_out = &pixels_vs2;
    }

    /*--------------------------*\
    | Increment background step  |
    \*--------------------------*/
    bkgd_step = bkgd_step + ((anim_speed * 100) / (100.0f * this->FPS));

    for(int ControllerID = 0; ControllerID < (int)Controllers.size(); ControllerID++)
    {
        RGBController* Controller = Controllers[ControllerID].Controller;
        if (ZoneMaps.size() != Controllers.size())
        {
            ZoneMaps.resize(Controllers.size());
            for (int ZM_ID = 0; ZM_ID < (int)ZoneMaps.size(); ZM_ID++)
            {
                ZoneMaps[ZM_ID].resize(Controllers[ControllerID].Controller->zones.size());
                for (int ZoneIndex = 0; ZoneIndex < (int)ZoneMaps[ZM_ID].size(); ZoneIndex++)
                {
                    ZoneMaps[ZM_ID][ZoneIndex] = nullptr;
                }
            }
        }

        for (int ZoneID = 0; ZoneID < (int)Controllers[ControllerID].OwnedZones.size(); ZoneID++)
        {
            int ZN = Controllers[ControllerID].OwnedZones[ZoneID];

            int                 x_count                 = Controller->zones[ZN].leds_count;
            int                 y_count                 = 0;
            zone_type           type                    = Controller->zones[ZN].type;
            ZoneIndexType *     zone_index_map          = NULL;

            /*--------------------------------------------------------------*\
            | If matrix type and matrix mapping is valid, get X and Y count  |
            \*--------------------------------------------------------------*/
            if(type == ZONE_TYPE_MATRIX)
            {
                x_count     = Controller->zones[ZN].matrix_map->width;
                y_count     = Controller->zones[ZN].matrix_map->height;
            }

            if (ZoneMaps[ControllerID][ZN] == nullptr)
            {
                ZoneIndexType *   new_index_map             = new ZoneIndexType();
                new_index_map->x_count                      = x_count;
                new_index_map->y_count                      = y_count;

                if(type == ZONE_TYPE_MATRIX)
                {
                    new_index_map->x_index                  = new int[x_count];
                    new_index_map->y_index                  = new int[y_count];

                    SetupMatrixGrid(x_count, y_count, new_index_map->x_index, new_index_map->y_index);
                }
                else if(type == ZONE_TYPE_LINEAR)
                {
                    new_index_map->x_index                  = new int[x_count];

                    SetupLinearGrid(x_count, new_index_map->x_index);
                }

                ZoneMaps[ControllerID][ZN] = new_index_map;
            }
            zone_index_map = ZoneMaps[ControllerID][ZN];

            switch (Controller->zones[ZN].type)
            {
            case ZONE_TYPE_MATRIX:
                for (int y = 0; y < y_count; y++)
                {
                    for (int x = 0; x < x_count; x++)
                    {
                        unsigned int map_idx = (y * x_count) + x;
                        unsigned int color_idx = Controller->zones[ZN].matrix_map->map[map_idx];
                        if( color_idx != 0xFFFFFFFF )
                        {
                            Controller->zones[ZN].colors[color_idx] = pixels_out->pixels[zone_index_map->y_index[y]][zone_index_map->x_index[x]];
                        }
                    }
                }
                break;

            case ZONE_TYPE_SINGLE:
                for (int r = 0; r < x_count; r++)
                {
                    Controller->zones[ZN].colors[r] = pixels_out->pixels[ROW_IDX_SINGLE_COLOR][0];
                }
                break;

            case ZONE_TYPE_LINEAR:
                for (int x = 0; x < x_count; x++)
                {
                    Controller->zones[ZN].colors[x] = pixels_out->pixels[ROW_IDX_BAR_GRAPH][zone_index_map->x_index[x]];
                }
                break;
            }
        }
    }
}
