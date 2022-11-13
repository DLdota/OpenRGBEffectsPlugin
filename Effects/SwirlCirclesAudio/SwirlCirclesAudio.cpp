#include "SwirlCirclesAudio.h"
#include "ColorUtils.h"
#include "AudioManager.h"

REGISTER_EFFECT(SwirlCirclesAudio);

SwirlCirclesAudio::SwirlCirclesAudio(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::SwirlCirclesAudio)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "SwirlCirclesAudio";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Rotating circles reacting to audio";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Glow";
    EffectDetails.HasCustomSettings = true;

    SetSpeed(50);
    SetSlider2Val(50);

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

SwirlCirclesAudio::~SwirlCirclesAudio()
{
    delete ui;
}


void SwirlCirclesAudio::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}


void SwirlCirclesAudio::StepEffect(std::vector<ControllerZone*> controller_zones)
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

    current_level = 0.f;

    for(int i = 0; i < 256; i++)
    {
        fft_fltr[i] = fft_fltr[i] + (filter_constant * (fft[i] - fft_fltr[i]));

        current_level += fft_fltr[i];
    }

    for(ControllerZone* controller_zone : controller_zones)
    {
        unsigned int start_idx = controller_zone->start_idx();
        bool reverse = controller_zone->reverse;
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            float hx = 0.5 * width;
            float hy = 0.5 * height;

            float y1 = hy + hy * sin(reverse ? -progress : progress);
            float x1 = hx + hx * cos(reverse ? -progress : progress);

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(i, 0, width, height, x1, y1);
                controller_zone->SetLED(start_idx + i, color, Brightness);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();
            unsigned int * map = controller_zone->map();

            float hx = 0.5 * width;
            float hy = 0.5 * height;

            float y1 = hy + hy * sin(reverse ? -progress : progress);
            float x1 = hx + hx * cos(reverse ? -progress : progress);

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    RGBColor color = GetColor(w, h, width, height, x1, y1);

                    unsigned int led_num = map[h * width + w];
                    controller_zone->SetLED(start_idx + led_num,color, Brightness);
                }
            }

        }
    }

    progress +=  0.1 * (float) Speed / (float) FPS;

    if(RandomColorsEnabled)
    {
        hsv1.hue++;
        hsv2.hue++;
    }

}

RGBColor SwirlCirclesAudio::GetColor(unsigned int x, unsigned int y, unsigned int w, unsigned int h, float x1, float y1)
{
    float glow_mult = 0.001 * Slider2Val;

    double distance1 = sqrt(pow(x1 - x, 2) + pow(y1 - y, 2));
    float distance1_percent = distance1 < radius ? 1 / (0.000001 + current_level) : pow(distance1 / (h+w), glow_mult * current_level);

    hsv_t hsv_tmp;

    hsv_tmp.value = std::min<float>(255 , std::max<float>(0, hsv1.value * (1 - distance1_percent)));
    hsv_tmp.hue = hsv1.hue;
    hsv_tmp.saturation = hsv1.saturation;

    RGBColor color1 = RGBColor(hsv2rgb(&hsv_tmp));

    float y2 = h - y1;
    float x2 = w - x1;

    double distance2 = sqrt(pow(x2 - x, 2) + pow(y2 - y, 2));
    float distance2_percent = distance2 < radius ? 1 / (0.000001 + current_level) : pow(distance2 / (h+w), glow_mult * current_level);

    hsv_tmp.value = std::min<float>(255 , std::max<float>(0, hsv2.value * (1 - distance2_percent)));
    hsv_tmp.hue = hsv2.hue;
    hsv_tmp.saturation = hsv2.saturation;

    RGBColor color2 = RGBColor(hsv2rgb(&hsv_tmp));

    return ColorUtils::Screen(color1, color2);
}

void SwirlCirclesAudio::SetUserColors(std::vector<RGBColor> colors)
{
    UserColors = colors;

    ResetUserColors();
}

void SwirlCirclesAudio::SetRandomColorsEnabled(bool value)
{
    RandomColorsEnabled = value;

    if(!RandomColorsEnabled)
    {
        ResetUserColors();
    }
    else
    {
        hsv1.hue = 0;
        hsv1.value = 255;
        hsv1.saturation = 255;

        hsv2.hue = 180;
        hsv2.value = 255;
        hsv2.saturation = 255;
    }
}


void SwirlCirclesAudio::ResetUserColors()
{
    rgb2hsv(UserColors[0], &hsv1);
    rgb2hsv(UserColors[1], &hsv2);
}


void SwirlCirclesAudio::EffectState(const bool state)
{
    EffectEnabled = state;
    state ? Start() : Stop();
}

void SwirlCirclesAudio::Start()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->RegisterClient(audio_device_idx, this);
    }
}

void SwirlCirclesAudio::Stop()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_device_idx, this);
    }
}

void SwirlCirclesAudio::on_devices_currentIndexChanged(int value)
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

void SwirlCirclesAudio::on_amplitude_valueChanged(int value)
{
    amplitude = value;
}

void SwirlCirclesAudio::on_decay_valueChanged(int value)
{
    decay = value;
}

void SwirlCirclesAudio::on_average_valueChanged(int value)
{
    avg_size = value;
}
void SwirlCirclesAudio::on_radius_valueChanged(int value)
{
    radius = value;
}

json SwirlCirclesAudio::SaveCustomSettings()
{
    json settings;

    settings["audio_device_idx"]    = audio_device_idx;
    settings["amplitude"]           = amplitude;
    settings["decay"]               = decay;
    settings["radius"]              = radius;

    return settings;
}

void SwirlCirclesAudio::LoadCustomSettings(json settings)
{
    if (settings.contains("audio_device_idx"))  ui->devices->setCurrentIndex(settings["audio_device_idx"]);
    if (settings.contains("amplitude"))         ui->amplitude->setValue(settings["amplitude"]);
    if (settings.contains("decay"))             ui->decay->setValue(settings["decay"]);
    if (settings.contains("radius"))            ui->radius->setValue(settings["radius"]);
}
