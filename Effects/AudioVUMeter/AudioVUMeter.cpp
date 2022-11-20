#include "AudioVUMeter.h"
#include "ColorUtils.h"
#include "AudioManager.h"
#include "hsv.h"

REGISTER_EFFECT(AudioVUMeter);

AudioVUMeter::AudioVUMeter(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::AudioVUMeter)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "AudioVUMeter";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Fill your led strip based on audio load";
    EffectDetails.MaxSpeed     = 20;
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
}

AudioVUMeter::~AudioVUMeter()
{
    delete ui;
}

void AudioVUMeter::EffectState(const bool state)
{
    EffectEnabled = state;
    state ? Start() : Stop();
}

void AudioVUMeter::Start()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->RegisterClient(audio_device_idx, this);
    }
}

void AudioVUMeter::Stop()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_device_idx, this);
    }
}

void AudioVUMeter::on_amplitude_valueChanged(int value)
{
    amplitude = value;
}

void AudioVUMeter::on_average_valueChanged(int value)
{
    avg_size = value;
}

void AudioVUMeter::on_decay_valueChanged(int value)
{
    decay = value;
}

void AudioVUMeter::on_color_offset_valueChanged(int value)
{
    color_offset = value;
}

void AudioVUMeter::on_color_spread_valueChanged(int value)
{
    color_spread = value;
}

void AudioVUMeter::on_saturation_valueChanged(int value)
{
    saturation = value;
}

void AudioVUMeter::on_devices_currentIndexChanged(int value)
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

void AudioVUMeter::StepEffect(std::vector<ControllerZone*> controller_zones)
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

    amp = std::min<float>(1.f,amp);

    if(last_height > amp)
    {
        last_height -= 0.1f * (float) Speed / (float) FPS;
        last_height = std::max<float>(0.f,last_height);
    }
    else
    {
        last_height = amp;
    }

    //printf("%f -- %f\n", amp, last_height);

    for(ControllerZone* controller_zone : controller_zones)
    {
        unsigned int start_idx = controller_zone->start_idx();
        zone_type ZT = controller_zone->type();
        bool reverse = controller_zone->reverse;

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(amp, reverse? width - i - 1 : i, width);
                controller_zone->SetLED(start_idx + i, color, Brightness);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();
            unsigned int * map = controller_zone->map();

            for(unsigned int h = 0; h < height; h++)
            {
                RGBColor color = GetColor(amp, reverse? height - h - 1 : h, height);

                for(unsigned int w = 0; w <  width; w++)
                {
                    unsigned int led_num = map[h * width + w];
                    controller_zone->SetLED(start_idx + led_num, color, Brightness);
                }
            }
        }
    }
}


RGBColor AudioVUMeter::GetColor(float a, float y, float h)
{
    float r_amp = a*h;
    float r_l_amp = last_height*h;

    hsv_t hsv;
    hsv.hue = int(color_offset + y/h*360*color_spread*0.01)%360;
    hsv.saturation = saturation;

    if(y <= r_amp)
    {
        hsv.value = 255;
        return RGBColor(hsv2rgb(&hsv));
    }

    if(fabs(y-r_l_amp) <= 1)
    {
        if(y < r_l_amp)
        {
            hsv.value = 255 * (1 - (r_l_amp-y));
        }
        else
        {
            hsv.value = 255 * (1 - (y - r_l_amp));
        }
        return RGBColor(hsv2rgb(&hsv));
    }

    return ColorUtils::OFF();
}


json AudioVUMeter::SaveCustomSettings()
{
    json settings;

    settings["audio_device_idx"] = audio_device_idx;
    settings["amplitude"] = amplitude;
    settings["avg_size"] = avg_size;
    settings["decay"] = decay;
    settings["color_offset"] = color_offset;
    settings["color_spread"] = color_spread;
    settings["saturation"] = saturation;

    return settings;
}

void AudioVUMeter::LoadCustomSettings(json Settings)
{
    if (Settings.contains("audio_device_idx"))
        ui->devices->setCurrentIndex(Settings["audio_device_idx"]);

    if (Settings.contains("amplitude"))
        ui->amplitude->setValue(Settings["amplitude"]);

    if (Settings.contains("avg_size"))
        ui->average->setValue(Settings["avg_size"]);

    if (Settings.contains("decay"))
        ui->decay->setValue(Settings["decay"]);

    if (Settings.contains("color_offset"))
        ui->color_offset->setValue(Settings["color_offset"]);

    if (Settings.contains("color_spread"))
        ui->color_spread->setValue(Settings["color_spread"]);

    if (Settings.contains("saturation"))
        ui->saturation->setValue(Settings["saturation"]);

}
