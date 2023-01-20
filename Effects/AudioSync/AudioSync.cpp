#include "AudioSync.h"
#include "math.h"
#include "ColorUtils.h"
#include "AudioManager.h"
#include "chuck_fft.h"
#include "ui_AudioSync.h"

REGISTER_EFFECT(AudioSync);

AudioSync::AudioSync(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::AudioSync)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Audio Sync";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Display frequency based colors with different modes";
    EffectDetails.HasCustomSettings = true;

    /*------------------------------------*\
    | Populate device list                 |
    \*------------------------------------*/
    std::vector<char *> devices = AudioManager::get()->GetAudioDevices();

    for(const char * str : devices)
    {
        ui->device->addItem(QString::fromUtf8(str));
    }

    /*------------------------------------*\
    | Populate presets list                |
    \*------------------------------------*/
    for(const AudioSyncSettings& preset: AudioSyncPresets)
    {
        ui->preset->addItem(preset.name.c_str());
    }

    /*------------------------------------*\
    | Populate average mode list           |
    \*------------------------------------*/
    ui->avg_mode->addItem("Binning");
    ui->avg_mode->addItem("Low pass");

    /*------------------------------------*\
    | Populate saturation mode list        |
    \*------------------------------------*/
    ui->saturation->addItem("No saturation");
    ui->saturation->addItem("Saturate high amplitudes");
    ui->saturation->addItem("Black and white mode");

    /*------------------------------------*\
    | Populate roll mode list              |
    \*------------------------------------*/
    ui->roll_mode->addItem("Linear");
    ui->roll_mode->addItem("No roll");
    ui->roll_mode->addItem("Radial");

    /*------------------------------------*\
    | Load defaults                        |
    \*------------------------------------*/
    ui->bypass->setMinimum(0);
    ui->bypass->setMaximum(256);

    LoadPreset(AudioSyncPresets[0]);

    /*--------------------------*\
    | Map signal to UpdateGraph  |
    \*--------------------------*/
    connect(this, SIGNAL(UpdateGraphSignal(QPixmap)), this, SLOT(UpdateGraph(QPixmap)));

    /*------------------------------------*\
    | Init preview                         |
    \*------------------------------------*/
    QImage image(256, 64, QImage::Format_RGB30);

    image.fill(Qt::black);

    QPixmap pixmap;
    pixmap.convertFromImage(image);

    ui->preview->setPixmap(pixmap);
    ui->preview->setScaledContents(true);

    /*------------------------------------*\
    | Fill in Normalization and FFT array  |
    \*------------------------------------*//*---------------------------------------------------------------------------------*\
    | Create a list of colors to read off of when drawing                               |
    \*---------------------------------------------------------------------------------*/
    hanning(win_hanning, 256);

    float offset            = 0.04f;
    float scale             = 0.5f;

    for (int i = 0; i < 256; i++)
    {
        fft[i] = 0.0f;
        fft_nrml[i] = offset + (scale * (i / 256.0f));
    }

    /*---------------------------------------------------------------------------------*\
    | Create a list of colors to read off of when drawing                               |
    \*---------------------------------------------------------------------------------*/
    int start_hue = 360;
    int stop_hue =  0;

    float hue_step = (stop_hue-start_hue)/256.0f;

    for(int i = 0; i<256;i++)
    {
        rainbow_hues.push_back(ceil(start_hue + i * hue_step));
    }
}

AudioSync::~AudioSync()
{
    Stop();
}

void AudioSync::StepEffect(std::vector<ControllerZone*> controller_zones)
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
        fft[i] = fft[i] *  0.01 *current_settings.decay / (60.f / FPS);
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
        fft_tmp[i] = (fft_tmp[i] - 128.0f) * ((amplitude) / 128.0f);
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

    if (current_settings.avg_mode == 0)
    {
        /*--------------------------------------------*\
        | Apply averaging over given number of values  |
        \*--------------------------------------------*/
        int k;
        float sum1 = 0;
        float sum2 = 0;
        for (k = 0; k < current_settings.avg_size; k++)
        {
            sum1 += fft[k];
            sum2 += fft[255 - k];
        }
        /*------------------------------*\
        | Compute averages for end bars  |
        \*------------------------------*/
        sum1 = sum1 / k;
        sum2 = sum2 / k;
        for (k = 0; k < current_settings.avg_size; k++)
        {
            fft[k] = sum1;
            fft[255 - k] = sum2;
        }
        for (int i = 0; i < (256 - current_settings.avg_size); i += current_settings.avg_size)
        {
            float sum = 0;
            for (int j = 0; j < current_settings.avg_size; j += 1)
            {
                sum += fft[i + j];
            }

            float avg = sum / current_settings.avg_size;

            for (int j = 0; j < current_settings.avg_size; j += 1)
            {
                fft[i + j] = avg;
            }
        }
    }
    else if(current_settings.avg_mode == 1)
    {
        for (int i = 0; i < current_settings.avg_size; i++)
        {
            float sum1 = 0;
            float sum2 = 0;
            int j;
            for (j = 0; j <= i + current_settings.avg_size; j++)
            {
                sum1 += fft[j];
                sum2 += fft[255 - j];
            }
            fft[i] = sum1 / j;
            fft[255 - i] = sum2 / j;
        }
        for (int i = current_settings.avg_size; i < 256 - current_settings.avg_size; i++)
        {
            float sum = 0;
            for (int j = 1; j <= current_settings.avg_size; j++)
            {
                sum += fft[i - j];
                sum += fft[i + j];
            }
            sum += fft[i];

            fft[i] = sum / (2 * current_settings.avg_size + 1);
        }
    }


    hsv_t HSVPixel;

    QImage image(256, 64, QImage::Format_RGB30);
    QPixmap pixmap;

    for(int i = 0; i < 256; i++)
    {
        fft_fltr[i] = fft_fltr[i] + (0.01 * current_settings.filter_constant * (fft[i] - fft_fltr[i]));

        if(i < 64)
        {
            fft_fltr[i] *= current_settings.low * 0.01;
        }
        else if ( i < 192)
        {
            fft_fltr[i] *= current_settings.middle * 0.01;
        }
        else
        {
            fft_fltr[i] *= current_settings.high * 0.01;
        }
        int shifted = (i+current_settings.rainbow_shift)%rainbow_hues.size();
        HSVPixel.hue = rainbow_hues[shifted];

        HSVPixel.saturation = (i >= current_settings.bypass_min && i <= current_settings.bypass_max) ? 255 : 128;
        HSVPixel.value = (i >= current_settings.bypass_min && i <= current_settings.bypass_max) ? 255 : 128;

        unsigned int color_value = hsv2rgb(&HSVPixel);

        RGBColor color = ToRGBColor(RGBGetBValue(color_value), RGBGetGValue(color_value), RGBGetRValue(color_value));

        float value = fft_fltr[i] * 100;

        for(int y = 0; y < 64; y++)
        {
            if(value > y || y == 0){
                image.setPixel(i, 63 - y, color);
            } else {
                image.setPixel(i, 63 - y, ColorUtils::OFF());
            }
        }
    }

    pixmap.convertFromImage(image);

    emit UpdateGraphSignal(pixmap);

    /*----------------------------------------------*\
    | find the main frequency in defined band width  |
    \*----------------------------------------------*/
    int max_idx = -1;
    float max_value = 0;

    for(int i = current_settings.bypass_min; i < current_settings.bypass_max; i++)
    {
        if(fft_fltr[i] > max_value )
        {
            max_value = fft_fltr[i];
            max_idx = i;
        }
    }

    hsv_t HSVVal;

    if(max_idx >= current_settings.bypass_min && max_idx <= current_settings.bypass_max)
    {
        int shifted = (max_idx+current_settings.rainbow_shift)%rainbow_hues.size();
        immediate_freq_hue = rainbow_hues[shifted];

        // slowly reach immediate
        if(current_freq_hue < immediate_freq_hue)
        {
            current_freq_hue += ((immediate_freq_hue - current_freq_hue) / (1.0f - (current_settings.fade_step / 100.f))) / FPS;
        }else
        {
            current_freq_hue -= ((current_freq_hue - immediate_freq_hue ) / (1.0f - (current_settings.fade_step / 100.f)) )/ FPS;
        }

        if(current_settings.saturation_mode == SATURATE_HIGH_AMPLITUDES)
        {
            if(max_value <= 0)
            {
                current_freq_sat += 1/(current_freq_sat * FPS);
            }
            else
            {
                current_freq_sat = 255 - 255 * pow(max_value, 3);
            }
        }
        else if(current_settings.saturation_mode == B_W_MODE)
        {
            current_freq_sat = 0;
        }
        else
        {
             current_freq_sat = 255;
        }

        current_freq_val = max_value * 255;
    }
    else
    {
        current_freq_sat-=1/(current_freq_sat * FPS);
        current_freq_val-=1/(current_freq_val * FPS);
    }

    current_freq_sat = current_freq_sat > 255 ? 255 : current_freq_sat < 0 ? 0: current_freq_sat;
    current_freq_val = current_freq_val > 255 ? 255 : current_freq_val < 0 ? 0: current_freq_val;

    HSVVal.hue = current_freq_hue;
    HSVVal.saturation = current_freq_sat;
    HSVVal.value = current_freq_val;

    RGBColor color = RGBColor(hsv2rgb(&HSVVal));

    colors_rotation.insert(colors_rotation.begin(), color);

    while(colors_rotation.size()>1024)
    {
        colors_rotation.pop_back();
    }

    int colors_count = (int)colors_rotation.size();

    for(ControllerZone* controller_zone: controller_zones)
    {
        /*-------------------*\
        | Setup for the loop  |
        \*-------------------*/
        int start_idx = controller_zone->start_idx();
        zone_type ZT = controller_zone->type();
        bool reverse = controller_zone->reverse;

        /*----------------------------------------------------*\
        | Adjust how it applies for the specific type of zone  |
        \*----------------------------------------------------*/
        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            int leds_count = controller_zone->leds_count();

            for (int LedID = 0; LedID < leds_count && LedID < colors_count; LedID++)
            {
                controller_zone->SetLED(start_idx + LedID, GetColor(1, reverse ? leds_count - LedID - 1 : LedID, leds_count, 1), Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            for (int col_id = 0; col_id < cols && col_id < colors_count; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zone->controller->zones[controller_zone->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zone->SetLED(start_idx + LedID, GetColor(reverse ? rows - row_id - 1: row_id, reverse ? cols - col_id - 1: col_id, rows, cols), Brightness);
                }
            }
        }
    }
}

RGBColor AudioSync::GetColor(int row, int col, int zone_width, int zone_height)
{
    switch(current_settings.roll_mode)
    {
    case RollMode::LINEAR:
        return colors_rotation[col];

    case RollMode::NONE:
        return colors_rotation[0];

    case RollMode::RADIAL:

        float center_x = (zone_width - 1) * 0.5;
        float center_y = (zone_height - 1) * 0.5;

        int distance = round(sqrt(pow(center_x - row, 2) + pow(center_y - col, 2)));

        return colors_rotation[distance];
    }

    return ColorUtils::OFF();
}


void AudioSync::EffectState(const bool State)
{
    State ? Start() : Stop();
}

void AudioSync::UpdateGraph(QPixmap pixmap)
{
    if(!is_running)
    {
        return;
    }

    ui->preview->setPixmap(pixmap);
    ui->preview->setScaledContents(true);
}

void AudioSync::Start()
{
    is_running = true;
    /*-----------------------------------------------------*\
    | The Audio Device defaults to -1 so we make sure that  |
    | it is greater than that before trying to register     |
    \*-----------------------------------------------------*/
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->RegisterClient(audio_device_idx, this);
    }
}

void AudioSync::Stop()
{
    is_running = false;
    /*-----------------------------------------------------*\
    | The Audio Device defaults to -1 so we make sure that  |
    | it is greater than that before trying to unregister   |
    \*-----------------------------------------------------*/
    if(audio_device_idx >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_device_idx, this);
    }

}

/*-----------------------------------------------------*\
| Load/Save json                                        |
\*-----------------------------------------------------*/
void AudioSync::LoadCustomSettings(json settings)
{
    if (settings.contains("audio_device_idx"))    ui->device->setCurrentIndex(settings["audio_device_idx"]);
    if (settings.contains("fade_step"))           ui->color_fade_speed->setValue(settings["fade_step"]);
    if (settings.contains("rainbow_shift"))       ui->hue_shift->setValue(settings["rainbow_shift"]);
    if (settings.contains("bypass_min"))          ui->bypass->setMinimumValue(settings["bypass_min"]);
    if (settings.contains("bypass_max"))          ui->bypass->setMaximumValue(settings["bypass_max"]);
    if (settings.contains("avg_size"))            ui->avg_size->setValue(settings["avg_size"]);
    if (settings.contains("avg_mode"))            ui->avg_mode->setCurrentIndex(settings["avg_mode"]);
    if (settings.contains("saturation_mode"))     ui->saturation->setCurrentIndex(settings["saturation_mode"]);
    if (settings.contains("roll_mode"))           ui->roll_mode->setCurrentIndex(settings["roll_mode"]);
    if (settings.contains("decay"))               ui->decay->setValue(settings["decay"]);
    if (settings.contains("filter_constant"))     ui->filter_constant->setValue(settings["filter_constant"]);
    if (settings.contains("low"))                 ui->low->setValue(settings["low"]);
    if (settings.contains("middle"))              ui->middle->setValue(settings["middle"]);
    if (settings.contains("high"))                ui->high->setValue(settings["high"]);
    if (settings.contains("amplitude"))           ui->amplitude->setValue(settings["amplitude"]);

    return;
}

json AudioSync::SaveCustomSettings()
{
    json settings;

    settings["audio_device_idx"]    = audio_device_idx;
    settings["amplitude"]           = amplitude;

    settings["fade_step"]           = current_settings.fade_step;
    settings["rainbow_shift"]       = current_settings.rainbow_shift;
    settings["bypass_min"]          = current_settings.bypass_min;
    settings["bypass_max"]          = current_settings.bypass_max;
    settings["avg_size"]            = current_settings.avg_size;
    settings["avg_mode"]            = current_settings.avg_mode;
    settings["roll_mode"]           = current_settings.roll_mode;
    settings["saturation_mode"]     = current_settings.saturation_mode;
    settings["decay"]               = current_settings.decay;
    settings["filter_constant"]     = current_settings.filter_constant;
    settings["high"]                = current_settings.high;
    settings["middle"]              = current_settings.middle;
    settings["low"]                 = current_settings.low;

    return settings;
}

/*-----------------------------------------------------*\
| UI bindings                                           |
\*-----------------------------------------------------*/
void AudioSync::on_device_currentIndexChanged(int value)
{
    bool was_running = is_running;

    if(is_running)
    {
        Stop();
    }

    audio_device_idx = value;

    if(was_running)
    {
        Start();
    }
}

void AudioSync::on_color_fade_speed_valueChanged(int value)
{
    current_settings.fade_step = value;
}

void AudioSync::on_hue_shift_valueChanged(int value)
{
    current_settings.rainbow_shift = value;
}

void AudioSync::on_bypass_valuesChanged(int min,int max)
{
    current_settings.bypass_min = min;
    current_settings.bypass_max = max;
}

void AudioSync::on_decay_valueChanged(int value)
{
    current_settings.decay = value;
}

void AudioSync::on_avg_size_valueChanged(int value)
{
    current_settings.avg_size = value;
}

void AudioSync::on_avg_mode_currentIndexChanged(int value)
{
    current_settings.avg_mode = value;
}

void AudioSync::on_saturation_currentIndexChanged(int value)
{
    current_settings.saturation_mode = value;
}

void AudioSync::on_roll_mode_currentIndexChanged(int value)
{
    current_settings.roll_mode = value;
}

void AudioSync::on_filter_constant_valueChanged(int value)
{
    current_settings.filter_constant = value;
}

void AudioSync::on_amplitude_valueChanged(int value)
{
    amplitude = value;
}

void AudioSync::on_low_valueChanged(int value)
{
    current_settings.low = value;
}

void AudioSync::on_middle_valueChanged(int value)
{
    current_settings.middle = value;
}

void AudioSync::on_high_valueChanged(int value)
{
    current_settings.high = value;
}

void AudioSync::on_defaults_clicked()
{
    LoadPreset(AudioSyncPresets[0]);
}

void AudioSync::on_preset_currentIndexChanged(int index)
{
    LoadPreset(AudioSyncPresets[index]);
}

void AudioSync::LoadPreset(AudioSyncSettings settings)
{
    ui->bypass->setValues(settings.bypass_min, settings.bypass_max);
    ui->hue_shift->setValue(settings.rainbow_shift);
    ui->color_fade_speed->setValue(settings.fade_step);
    ui->decay->setValue(settings.decay);
    ui->avg_size->setValue(settings.avg_size);
    ui->avg_mode->setCurrentIndex(settings.avg_mode);
    ui->roll_mode->setCurrentIndex(settings.roll_mode);
    ui->saturation->setCurrentIndex(settings.saturation_mode);
    ui->filter_constant->setValue(settings.filter_constant);
    ui->low->setValue(settings.low);
    ui->middle->setValue(settings.middle);
    ui->high->setValue(settings.high);
}
