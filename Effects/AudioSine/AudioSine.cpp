#include "AudioSine.h"
#include "ColorUtils.h"
#include "AudioManager.h"

REGISTER_EFFECT(AudioSine);

AudioSine::AudioSine(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::AudioSine)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "AudioSine";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Sinusoidal audio rendering";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.HasCustomSettings = true;

    SetSpeed(50);

    std::vector<char *> devices = AudioManager::get()->GetAudioDevices();

    for(const char * str : devices)
    {
        ui->devices->addItem(QString::fromUtf8(str));
    }

    hanning(win_hanning, 256);

    float offset            = 0.04f;
    float scale             = 0.5f;

    /*------------------------------------*\
    | Fill in Normalization and FFT array  |
    \*------------------------------------*/
    for (int i = 0; i < 256; i++)
    {
        fft[i] = 0.0f;
        fft_nrml[i] = offset + (scale * (i / 256.0f));
    }

    ui->color_mode->addItems({"Spectrum cycle", "Static"});
    ui->background->SetRGBColor(background);
    ui->wave_color->SetRGBColor(wave_color);
}

AudioSine::~AudioSine()
{
    delete ui;
}

void AudioSine::EffectState(const bool state)
{
    EffectEnabled = state;
    state ? Start() : Stop();
}

void AudioSine::Start()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->RegisterClient(audio_device_idx, this);
    }
}

void AudioSine::Stop()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_device_idx, this);
    }
}

void AudioSine::on_color_mode_currentIndexChanged(int value)
{
    color_mode = value;
}

void AudioSine::on_devices_currentIndexChanged(int value)
{
    bool was_running = EffectEnabled;

    if(EffectEnabled)
    {
        Stop();
    }

    audio_device_idx = value;

    if(was_running)
    {
        Start();
    }
}


void AudioSine::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void AudioSine::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    float fft_tmp[512];

    for (int i = 0; i < 256; i++)
    {
        /*------------------*\
        | Clear the buffers  |
        \*------------------*/
        fft_tmp[i] = 0;

        /*----------------------*\
        | Decay previous values  |
        \*----------------------*/
        fft[i] = fft[i] * ((float(decay) / 100.0f / (60 / FPS)));
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

    apply_window(fft_tmp, win_hanning, 256);

    /*------------------------*\
    | Run the FFT calculation  |    void EffectState(bool) override;

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

        /*--------------------------------------------------------------------*\
        | Set odd indexes to match their corresponding even index, as the FFT  |
        | input array uses two indices for one value (real+imaginary)          |
        \*--------------------------------------------------------------------*/
        fft[(i * 2) + 1] = fft[i * 2];
        fft[(i * 2) + 2] = fft[i * 2];
        fft[(i * 2) + 3] = fft[i * 2];
    }


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

    for(int i = 0; i < 256; i++)
    {
        fft_fltr[i] = fft_fltr[i] + (filter_constant * (fft[i] - fft_fltr[i]));
    }

    float amp = 0;

    for(int i = 0; i < 256; i += avg_size)
    {
        amp += fft_fltr[i];
    }

    for(ControllerZone* controller_zone : controller_zones)
    {
        unsigned int start_idx = controller_zone->start_idx();
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            for(unsigned int i = 0; i < width; i++)
            {
                float sine_value = GetSineValue(i, width, controller_zone->reverse);

                RGBColor color = GetColor(sine_value, 0, height);
                controller_zone->SetLED(start_idx + i, color, Brightness);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();
            unsigned int * map = controller_zone->map();

            for(unsigned int w = 0; w <  width; w++)
            {
                float sine_value = GetSineValue(w, width, controller_zone->reverse);

                for(unsigned int h = 0; h < height; h++)
                {
                    RGBColor color = GetColor(sine_value, h, height);

                    unsigned int led_num = map[h * width + w];
                    controller_zone->SetLED(start_idx + led_num, color, Brightness);
                }
            }
        }
    }

    x_time += (float) Speed / (float) FPS;
    oscillation_time += (float) oscillation / (float) FPS;
    color_time += (float) color_change_speed / (float) FPS;

    height_mult = oscillation > 0 ? sin(oscillation_time * 0.1) : 1;
}

float AudioSine::GetSineValue(unsigned int x, unsigned int width, bool reverse)
{
    float value = 0;
    double pi = 3.14159265359;

    x++;
    width++;

    if(Speed > 0)
    {
        x = reverse ? x - x_time : x + x_time;
    }

    float x_percent = (float)x / (float)width;

    for (int i = 0; i < 256; i+= avg_size)
    {
        value += height_mult * fft[i] * sin(x_percent * 0.25 * repeat * (i / (float) avg_size) * pi);
    }

    return value;
}

RGBColor AudioSine::GetColor(float sine, unsigned int y, unsigned int height)
{
    float half_height = height * 0.5;
    float peak = half_height + sine * half_height;

    float real_distance = fabs(peak - y);

    float distance = real_distance > 0.01 * thickness * height ?
                pow(real_distance / (float)height, 0.01 * glow)
              : 0;

    distance = std::min<float>(std::max<float>(distance, 0), 1);

    RGBColor color;

    if(color_mode == 0)
    {
        hsv_t hsv;
        hsv.hue = (int) color_time % 360;
        hsv.saturation = 255;
        hsv.value = 255 - 255 * distance;

        color = RGBColor(hsv2rgb(&hsv));
    }
    else
    {
       color = ColorUtils::Enlight(wave_color, 1 - distance);
    }

    return ColorUtils::Interpolate(color, background, distance);
}


void AudioSine::on_amplitude_valueChanged(int value)
{
    amplitude = value;
}

void AudioSine::on_repeat_valueChanged(int value)
{
    repeat = value;
}

void AudioSine::on_glow_valueChanged(int value)
{
    glow = value;
}

void AudioSine::on_thickness_valueChanged(int value)
{
    thickness = value;
}

void AudioSine::on_average_valueChanged(int value)
{
    avg_size = value;
}

void AudioSine::on_oscillation_valueChanged(int value)
{
    oscillation = value;
}

void AudioSine::on_color_change_speed_valueChanged(int value)
{
    color_change_speed = value;
}


void AudioSine::on_background_ColorSelected(QColor color)
{
    background = ColorUtils::fromQColor(color);
}

void AudioSine::on_wave_color_ColorSelected(QColor color)
{
    wave_color = ColorUtils::fromQColor(color);
}

json AudioSine::SaveCustomSettings()
{
    json settings;

    settings["audio_device_idx"] = audio_device_idx;
    settings["color_mode"] = color_mode;
    settings["amplitude"] = amplitude;
    settings["avg_size"] = avg_size;
    settings["repeat"] = repeat;
    settings["glow"] = glow;
    settings["thickness"] = thickness;
    settings["oscillation"] = oscillation;
    settings["color_change_speed"] = color_change_speed;
    settings["background"] = background;
    settings["wave_color"] = wave_color;
    
    return settings;
}

void AudioSine::LoadCustomSettings(json Settings)
{
    if (Settings.contains("audio_device_idx"))
        ui->devices->setCurrentIndex(Settings["audio_device_idx"]);

    if (Settings.contains("color_mode"))
        ui->color_mode->setCurrentIndex(Settings["color_mode"]);

    if (Settings.contains("amplitude"))
        ui->amplitude->setValue(Settings["amplitude"]);

    if (Settings.contains("avg_size"))
        ui->average->setValue(Settings["avg_size"]);

    if (Settings.contains("repeat"))
        ui->repeat->setValue(Settings["repeat"]);

    if (Settings.contains("glow"))
        ui->glow->setValue(Settings["glow"]);

    if (Settings.contains("thickness"))
        ui->thickness->setValue(Settings["thickness"]);

    if (Settings.contains("oscillation"))
        ui->oscillation->setValue(Settings["oscillation"]);

    if (Settings.contains("color_change_speed"))
        ui->color_change_speed->setValue(Settings["color_change_speed"]);

    if (Settings.contains("background"))
    {
        background = Settings["background"];
        ui->background->SetRGBColor(Settings["background"]);
    }

    if (Settings.contains("wave_color"))
    {
        wave_color = Settings["wave_color"];
        ui->wave_color->SetRGBColor(Settings["wave_color"]);
    }
}
