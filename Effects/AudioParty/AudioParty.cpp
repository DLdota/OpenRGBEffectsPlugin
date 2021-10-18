#include "AudioParty.h"
#include "ColorUtils.h"
#include "AudioManager.h"

REGISTER_EFFECT(AudioParty);

AudioParty::AudioParty(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::AudioParty)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "AudioParty";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Movement, color change, effects according to audio zones. <br/> - Blue zone: motion<br/>- Green zone: colors<br/>- Red zone effects";
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.MaxSlider2Val = 50;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Color change speed";
    EffectDetails.HasCustomSettings = true;

    SetSpeed(50);
    SetSlider2Val(25);

    std::vector<char *> devices = AudioManager::get()->GetAudioDevices();

    for(const char * str : devices)
    {
        ui->devices->addItem(QString::fromLocal8Bit(str));
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

    color = ColorUtils::RandomRGBColor();

    connect(this, SIGNAL(UpdateGraphSignal(QPixmap)), this, SLOT(UpdateGraph(QPixmap)));

    ui->preview->setScaledContents(true);

    void on_effect_threshold_valueChanged(double);

    zones_slider = new ctkRangeSlider(Qt::Horizontal);

    zones_slider->setSingleStep(8);

    zones_slider->setMaximum(0);
    zones_slider->setMaximum(32);

    zones_slider->setValues(motion_zone_stop, color_zone_stop);

    ui->zones_layout->addWidget(zones_slider);

    connect(zones_slider,SIGNAL(valuesChanged(int,int)), this, SLOT(ZonesChanged(int, int)));

}

AudioParty::~AudioParty()
{
    delete ui;
}

void AudioParty::EffectState(const bool state)
{
    EffectEnabled = state;
    state ? Start() : Stop();
}

void AudioParty::Start()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->RegisterClient(audio_device_idx, this);
    }
}

void AudioParty::Stop()
{
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_device_idx, this);
    }
}

void AudioParty::on_devices_currentIndexChanged(int value)
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

void AudioParty::UpdateGraph(QPixmap pixmap)
{
    ui->preview->setPixmap(pixmap);
}

void AudioParty::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void AudioParty::StepEffect(std::vector<ControllerZone*> controller_zones)
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
    unsigned int k;
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

    for (unsigned int i = 0; i < (256 - avg_size); i += avg_size)
    {
        float sum = 0;
        for (unsigned int j = 0; j < avg_size; j += 1)
        {
            sum += fft[i + j];
        }

        float avg = sum / avg_size;

        for (unsigned int j = 0; j < avg_size; j += 1)
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

    /*--------------------------------------------*\
    | Generate preview                             |
    \*--------------------------------------------*/
    RGBColor color;
    QImage image(256, 64, QImage::Format_RGB30);
    QPixmap pixmap;

    for(unsigned int i = 0; i < 256; i++)
    {
        float value = fft_fltr[i] * 100;

        color = i < motion_zone_stop * avg_size ?
                    ToRGBColor(255,0,0) : i < color_zone_stop * avg_size ?
                        ToRGBColor(0,255,0) : ToRGBColor(0, 0, 255);

        for(int y = 0; y < 64; y++)
        {
            if(y==round(effect_threshold*100)){
                image.setPixel(i, 63 - y, ToRGBColor(255,255,255));
            }
            else if(value > y || y == 0){
                image.setPixel(i , 63 - y, color);
            } else {
                image.setPixel(i, 63 - y, ColorUtils::OFF());
            }
        }

    }

    pixmap.convertFromImage(image);

    emit UpdateGraphSignal(pixmap);

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
                RGBColor color = GetColor(i, 0, width, height);
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
                    RGBColor color = GetColor(w, h, width, height);

                    unsigned int led_num = map[h * width + w];
                    controller_zone->SetLED(start_idx + led_num, color, Brightness);
                }
            }
        }
    }

    float delta = (float) Speed / (float) FPS;
    float color_delta = (float) Slider2Val / (float) FPS;

    /*--------------------------------------------*\
    | X axis motion                                |
    \*--------------------------------------------*/

    int c = 0;

    for (unsigned int i = 0; i < motion_zone_stop * avg_size ; i+= avg_size)
    {
        if(fft[i] > fft[i+avg_size]){
            int mult = c % 2 == 0 ? 1:-1;
            x_shift += fft[i] * delta * mult;
            break;
        }
        c++;
    }

    /*--------------------------------------------*\
    | Color motion                                 |
    \*--------------------------------------------*/
    c = 0;
    for (unsigned int i = motion_zone_stop * avg_size; i < color_zone_stop * avg_size ; i+= avg_size)
    {
        if(fft[i] > fft[i+avg_size]){
            int mult = c % 2 == 0 ? 1:-1;
            color_shift += fft[i] * color_delta * mult;
            break;
        }
        c++;
    }

    /*--------------------------------------------*\
    | Trigger effects                              |
    \*--------------------------------------------*/
    for (unsigned int i = color_zone_stop * avg_size; i < 31 * avg_size; i+= avg_size)
    {
        if(fft[i] > effect_threshold && fft[i] > fft[i+avg_size] && effect_progress >= 1){
            effect_idx = rand()%7;
            effect_progress = 0;
            break;
        }
        c++;
    }

    /*--------------------------------------------*\
    | Run effects                                  |
    \*--------------------------------------------*/
    if(effect_progress < 1)
    {
        effect_progress += 0.1 * delta;
    }

}


RGBColor AudioParty::GetColor(float x, float y, float w, float h)
{
    float pi = 3.14159265359;
    float pos_x = x / w;
    float s = 0.5 * (1 + sin(pos_x * divisions * pi + x_shift));

    /*--------------------------------------------*\
    | Color motion                                 |
    \*--------------------------------------------*/
    hsv_t hsv;
    hsv.hue = 180 + sin(y/h + color_shift) * 180;
    hsv.saturation = 255;
    hsv.value = 255;

    RGBColor effect_color = ColorUtils::OFF();

    if(effect_progress < 1)
    {
        switch (effect_idx) {
        case 0: {
            // moving bar LTR

            if(fabs(x - effect_progress * w) <= 1){
                effect_color = ToRGBColor(255,255,255);
            }

            break;
        }
        case 1: {
            // moving bar RTL

            if(fabs(x - (w - effect_progress * w)) <= 1){
                effect_color = ToRGBColor(255,255,255);
            }

            break;
        }
        case 2: {
            // moving bar UTB


            if(fabs(y - effect_progress * h) <= 1){
                effect_color = ToRGBColor(255,255,255);
            }

            break;
        }
        case 3: {
            // moving bar BTU

            if(fabs(y - (h - effect_progress * h)) <= 1){
                effect_color = ToRGBColor(255,255,255);
            }

            break;
        }
        case 4: {
            // random rgb
            effect_color = ColorUtils::RandomRGBColor();
            break;
        }
        case 5: {
            // random white
            float value = ((double) rand() / (RAND_MAX));
            effect_color = ColorUtils::Enlight(ToRGBColor(255,255,255), value);

            break;
        }
        case 6: {
            // blink
            float value = 1 - effect_progress;
            effect_color = ColorUtils::Enlight(ToRGBColor(255,255,255), value);

            break;
        }


        default: break;

        }
    }

    RGBColor c = ColorUtils::Screen(hsv2rgb(&hsv),effect_color);

    return ColorUtils::Interpolate(c, ColorUtils::OFF(), s);
}

void AudioParty::on_amplitude_valueChanged(int value)
{
    amplitude = value;
}

void AudioParty::on_divisions_valueChanged(int value)
{
    divisions = value;
}

void AudioParty::on_effect_threshold_valueChanged(double value)
{
    effect_threshold = value;
}

void AudioParty::ZonesChanged(int min, int max)
{
    motion_zone_stop = min;
    color_zone_stop = max;
}

json AudioParty::SaveCustomSettings(json Settings)
{
    Settings["audio_device_idx"] = audio_device_idx;
    Settings["amplitude"] = amplitude;
    Settings["divisions"] = divisions;
    Settings["effect_threshold"] = effect_threshold;
    Settings["motion_zone_stop"] = motion_zone_stop;
    Settings["color_zone_stop"] = color_zone_stop;

    return Settings;
}

void AudioParty::LoadCustomSettings(json Settings)
{
    if (Settings.contains("audio_device_idx"))
        ui->devices->setCurrentIndex(Settings["audio_device_idx"]);

    if (Settings.contains("amplitude"))
        ui->amplitude->setValue(Settings["amplitude"]);

    if (Settings.contains("divisions"))
        ui->divisions->setValue(Settings["divisions"]);

    if (Settings.contains("effect_threshold"))
        ui->effect_threshold->setValue(Settings["effect_threshold"]);

    if (Settings.contains("motion_zone_stop"))
        zones_slider->setMinimumValue(Settings["motion_zone_stop"]);

    if (Settings.contains("color_zone_stop"))
        zones_slider->setMaximumValue(Settings["color_zone_stop"]);
}
