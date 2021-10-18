#include "AudioStar.h"
#include "ColorUtils.h"
#include "AudioManager.h"

REGISTER_EFFECT(AudioStar);

AudioStar::AudioStar(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::AudioStar)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "AudioStar";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Star audio visualizer";
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

}

AudioStar::~AudioStar()
{
    delete ui;
}

void AudioStar::EffectState(const bool state)
{
    EffectEnabled = state;
    state ? Start() : Stop();
}

void AudioStar::Start()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->RegisterClient(audio_device_idx, this);
    }
}

void AudioStar::Stop()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_device_idx, this);
    }
}

void AudioStar::on_devices_currentIndexChanged(int value)
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


void AudioStar::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void AudioStar::StepEffect(std::vector<ControllerZone*> controller_zones)
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

    amp = 0;

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
                RGBColor color = GetColor(i, 0.5, width, height);
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
                for(unsigned int h = 0; h < height; h++)
                {
                    RGBColor color = GetColor(w, h, width-1, height-1);

                    unsigned int led_num = map[h * width + w];
                    controller_zone->SetLED(start_idx + led_num, color, Brightness);
                }
            }
        }
    }

    time += (float) Speed / (float) FPS;
}

RGBColor AudioStar::GetColor(float x, float y, float w, float h)
{
    hsv_t hsv;

    float cx = w * 0.5;
    float cy = h * 0.5;

    float pi = 3.14;

    float angle = fabs(atan2((x - cx), (y  - cy)));

    double freq_amp = fft_fltr[(int)(256 * (angle / (pi*2)))];

    hsv.hue = (int)((angle/pi)*360 + time) % 360;
    hsv.saturation = 255;
    hsv.value = std::min<float>(255, pow(freq_amp, 1/(amp+1)) * 255);

    RGBColor color = RGBColor(hsv2rgb(&hsv));

    if(edge_beat)
    {
        bool is_edge = x == 0 || x == w || y == 0 || y == h ;

        if (is_edge)
        {
            hsv_t edge;

            edge.hue = 0;
            edge.saturation = 0;
            edge.value = std::min<float>(255, (fft_fltr[0] + fft_fltr[8]) * 255);

            return ColorUtils::Screen(color, RGBColor(hsv2rgb(&edge)));
        }
    }

    return color;
}


void AudioStar::on_amplitude_valueChanged(int value)
{
    amplitude = value;
}

void AudioStar::on_decay_valueChanged(int value)
{
    decay = value;
}

void AudioStar::on_edge_beat_stateChanged(int value)
{
    edge_beat = value;
}

json AudioStar::SaveCustomSettings(json Settings)
{
    Settings["audio_device_idx"] = audio_device_idx;
    Settings["amplitude"] = amplitude;
    Settings["decay"] = decay;
    Settings["edge_beat"] = edge_beat;
    return Settings;
}

void AudioStar::LoadCustomSettings(json Settings)
{
    if (Settings.contains("audio_device_idx"))
        ui->devices->setCurrentIndex(Settings["audio_device_idx"]);

    if (Settings.contains("amplitude"))
        ui->amplitude->setValue(Settings["amplitude"]);

    if (Settings.contains("decay"))
        ui->decay->setValue(Settings["decay"]);

    if (Settings.contains("edge_beat"))
        ui->edge_beat->setChecked(Settings["edge_beat"]);
}
