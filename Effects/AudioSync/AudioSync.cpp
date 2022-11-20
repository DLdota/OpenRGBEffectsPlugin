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
    | Fill in Normalization and FFT array  |
    \*------------------------------------*/
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

/*

void AudioSync::CreateUi()
{
    PrimaryFrame = new QFrame();
    QVBoxLayout* PrimaryLayout = new QVBoxLayout();

    AudioSyncFrame = new QFrame();
    QVBoxLayout* MainAudioSyncLayout = new QVBoxLayout;

    int label_width = 120;

    preview = new QLabel();
    preview->setScaledContents(true);
    preview->setMinimumHeight(64);
    preview->setMinimumWidth(256);

    MainAudioSyncLayout->addWidget(preview);

    QHBoxLayout* DeviceListLayout = new QHBoxLayout;
    const QString device_tooltip = "Switch audio device";
    QLabel *device_label = new QLabel("Device");

    device_label->setFixedWidth(label_width);

    device_label->setToolTip(device_tooltip);
    device_list_selector->setToolTip(device_tooltip);
    device_list_selector->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

    connect(device_list_selector, SIGNAL(currentIndexChanged(int)), this, SLOT(SelectDeviceChanged(int)));

    DeviceListLayout->addWidget(device_label);
    DeviceListLayout->addWidget(device_list_selector);
    MainAudioSyncLayout->addLayout(DeviceListLayout);

    QHBoxLayout* BPFilterLayout = new QHBoxLayout;
    const QString bw_tooltip = "Cut off frenquencies";
    QLabel *bw_label = new QLabel("Band-pass filter");
    bw_label->setFixedWidth(label_width);
    connect(bypass_slider,SIGNAL(valuesChanged(int,int)), this, SLOT(BypassChanged(int, int)));

    bw_label->setToolTip(bw_tooltip);
    bypass_slider->setToolTip(bw_tooltip);

    BPFilterLayout->addWidget(bw_label);
    BPFilterLayout->addWidget(bypass_slider);
    MainAudioSyncLayout->addLayout(BPFilterLayout);

    QHBoxLayout* RBShiftLayout = new QHBoxLayout;
    const QString rainbow_shift_tooltip = "Rainbow shift";
    QLabel *rainbow_shift_label = new QLabel("Shift the rainbow");
    rainbow_shift_label->setFixedWidth(label_width);
    rainbow_shift_slider->setMinimum(0);
    rainbow_shift_slider->setMaximum(256);
    rainbow_shift_slider->setOrientation(Qt::Horizontal);

    connect(rainbow_shift_slider,SIGNAL(valueChanged(int)), this, SLOT(RainbowShiftChanged(int)));

    rainbow_shift_label->setToolTip(rainbow_shift_tooltip);
    rainbow_shift_slider->setToolTip(rainbow_shift_tooltip);

    RBShiftLayout->addWidget(rainbow_shift_label);
    RBShiftLayout->addWidget(rainbow_shift_slider);
    MainAudioSyncLayout->addLayout(RBShiftLayout);

    QHBoxLayout* CFadeLayout = new QHBoxLayout;
    const QString fade_tooltip = "Hue step value for switching colors";
    QLabel *fade_label = new QLabel("Color fade speed");
    fade_label->setFixedWidth(label_width);
    fade_step_slider->setMinimum(0);
    fade_step_slider->setMaximum(99);
    fade_step_slider->setOrientation(Qt::Horizontal);

    connect(fade_step_slider,SIGNAL(valueChanged(int)), this, SLOT(FadeStepChanged(int)));

    fade_label->setToolTip(fade_tooltip);
    fade_step_slider->setToolTip(fade_tooltip);

    CFadeLayout->addWidget(fade_label);
    CFadeLayout->addWidget(fade_step_slider);
    MainAudioSyncLayout->addLayout(CFadeLayout);


    QHBoxLayout* DecayLayout = new QHBoxLayout;
    const QString decay_tooltip = "Decay signals, low value = rapid decay, high value = slow decay";
    QLabel *decay_label = new QLabel("Decay");
    decay_label->setFixedWidth(label_width);
    decay_slider->setMinimum(1);
    decay_slider->setMaximum(99);
    decay_slider->setOrientation(Qt::Horizontal);

    connect(decay_slider,SIGNAL(valueChanged(int)), this, SLOT(DecayChanged(int)));

    decay_label->setToolTip(decay_tooltip);
    decay_slider->setToolTip(decay_tooltip);

    DecayLayout->addWidget(decay_label);
    DecayLayout->addWidget(decay_slider);
    MainAudioSyncLayout->addLayout(DecayLayout);

    QHBoxLayout* AVRGSizeLayout = new QHBoxLayout;
    const QString avg_size_tooltip = "How many windows to keep in memory for averaging the signal";
    QLabel *avg_size_label = new QLabel("Avg size");
    avg_size_label->setFixedWidth(label_width);
    avg_size_slider->setMinimum(1);
    avg_size_slider->setMaximum(64);
    avg_size_slider->setOrientation(Qt::Horizontal);

    connect(avg_size_slider,SIGNAL(valueChanged(int)), this, SLOT(AvgSizeChanged(int)));

    avg_size_label->setToolTip(avg_size_tooltip);
    avg_size_slider->setToolTip(avg_size_tooltip);

    AVRGSizeLayout->addWidget(avg_size_label);
    AVRGSizeLayout->addWidget(avg_size_slider);
    MainAudioSyncLayout->addLayout(AVRGSizeLayout);

    QHBoxLayout* AVRGModeLayout = new QHBoxLayout;
    const QString avg_mode_tooltip = "Switch average modes";
    QLabel *avg_mode_label = new QLabel("Avg mode");
    avg_mode_label->setFixedWidth(label_width);
    avg_mode_selector->addItem("Binning");
    avg_mode_selector->addItem("Low pass");

    connect(avg_mode_selector, SIGNAL(currentIndexChanged(int)), this, SLOT(AvgModeChanged(int)));

    avg_mode_label->setToolTip(avg_mode_tooltip);
    avg_mode_selector->setToolTip(avg_mode_tooltip);

    AVRGModeLayout->addWidget(avg_mode_label);
    AVRGModeLayout->addWidget(avg_mode_selector);
    MainAudioSyncLayout->addLayout(AVRGModeLayout);

    QHBoxLayout* FiltConstLayout = new QHBoxLayout;
    const QString filter_constant_tooltip = "Filter constant, TODO";
    QLabel *filter_constant_label = new QLabel("Filter constant");
    filter_constant_label->setFixedWidth(label_width);
    filter_constant_slider->setMinimum(0);
    filter_constant_slider->setMaximum(100);
    filter_constant_slider->setOrientation(Qt::Horizontal);

    connect(filter_constant_slider,SIGNAL(valueChanged(int)), this, SLOT(FilterConstantChanged(int)));

    filter_constant_label->setToolTip(filter_constant_tooltip);
    filter_constant_slider->setToolTip(filter_constant_tooltip);

    FiltConstLayout->addWidget(filter_constant_label);
    FiltConstLayout->addWidget(filter_constant_slider);
    MainAudioSyncLayout->addLayout(FiltConstLayout);


    QHBoxLayout* SaturationModeLayout = new QHBoxLayout;
    const QString saturation_mode_tooltip = "Saturation on high amplitudes";
    QLabel *saturation_mode_label = new QLabel("Saturation");
    saturation_mode_label->setFixedWidth(label_width);
    saturation_mode_selector->addItem("No saturation");
    saturation_mode_selector->addItem("Saturate high amplitudes");

    connect(saturation_mode_selector, SIGNAL(currentIndexChanged(int)), this, SLOT(SaturationModeChanged(int)));

    saturation_mode_label->setToolTip(saturation_mode_tooltip);
    saturation_mode_selector->setToolTip(saturation_mode_tooltip);

    SaturationModeLayout->addWidget(saturation_mode_label);
    SaturationModeLayout->addWidget(saturation_mode_selector);
    MainAudioSyncLayout->addLayout(SaturationModeLayout);


    QHBoxLayout* RollModeLayout = new QHBoxLayout;
    const QString roll_mode_tooltip = "Change the roll mode";
    QLabel *roll_mode_label = new QLabel("Roll mode");
    roll_mode_label->setFixedWidth(label_width);
    roll_mode_selector->addItem("Linear");
    roll_mode_selector->addItem("No roll");
    roll_mode_selector->addItem("Radial");

    connect(roll_mode_selector, SIGNAL(currentIndexChanged(int)), this, SLOT(RollModeChanged(int)));

    roll_mode_label->setToolTip(roll_mode_tooltip);
    roll_mode_selector->setToolTip(roll_mode_tooltip);

    RollModeLayout->addWidget(roll_mode_label);
    RollModeLayout->addWidget(roll_mode_selector);
    MainAudioSyncLayout->addLayout(RollModeLayout);


    QHBoxLayout* AmpLayout = new QHBoxLayout;
    const QString amplitude_tooltip = "Gain multiplicater, affects all frequencies";
    QLabel *amplitude_label = new QLabel("Amplitude");
    amplitude_label->setFixedWidth(label_width);
    amplitude_slider->setOrientation(Qt::Horizontal);

    amplitude_slider->setMinimum(0);
    amplitude_slider->setMaximum(100);

    amplitude_slider_min->setFixedHeight(22);
    amplitude_slider_max->setFixedHeight(22);

    amplitude_slider_min->setMaximum(2147483646);
    amplitude_slider_max->setMaximum(2147483647);

    amplitude_slider_min->hide();
    amplitude_slider_max->hide();

    connect(amplitude_slider,SIGNAL(valueChanged(int)), this, SLOT(AmplitudeChanged(int)));

    amplitude_label->setToolTip(amplitude_tooltip);
    amplitude_slider->setToolTip(amplitude_tooltip);

    change_amplitude_button = new QPushButton("Edit");

    connect(change_amplitude_button,SIGNAL(clicked(bool)), this, SLOT(ToggleAmplitudeChangeInputs()));

    AmpLayout->addWidget(amplitude_label);
    AmpLayout->addWidget(change_amplitude_button);
    AmpLayout->addWidget(amplitude_slider_min);
    AmpLayout->addWidget(amplitude_slider);
    AmpLayout->addWidget(amplitude_slider_max);

    MainAudioSyncLayout->addLayout(AmpLayout);


    QHBoxLayout* LowLayout = new QHBoxLayout;
    const QString low_tooltip = "Low frequencies gain";
    QLabel *low_label = new QLabel("Low");
    low_label->setFixedWidth(label_width);
    low_slider->setMinimum(0);
    low_slider->setMaximum(200);
    low_slider->setOrientation(Qt::Horizontal);

    connect(low_slider,SIGNAL(valueChanged(int)), this, SLOT(LowChanged(int)));

    low_label->setToolTip(low_tooltip);
    low_slider->setToolTip(low_tooltip);

    LowLayout->addWidget(low_label);
    LowLayout->addWidget(low_slider);
    MainAudioSyncLayout->addLayout(LowLayout);


    QHBoxLayout* MidLayout = new QHBoxLayout;
    const QString middle_tooltip = "Middle frequencies gain";
    QLabel *middle_label = new QLabel("Middle");
    middle_label->setFixedWidth(label_width);
    middle_slider->setMinimum(0);
    middle_slider->setMaximum(200);
    middle_slider->setOrientation(Qt::Horizontal);

    connect(middle_slider,SIGNAL(valueChanged(int)), this, SLOT(MiddleChanged(int)));

    middle_label->setToolTip(middle_tooltip);
    middle_slider->setToolTip(middle_tooltip);

    MidLayout->addWidget(middle_label);
    MidLayout->addWidget(middle_slider);
    MainAudioSyncLayout->addLayout(MidLayout);


    QHBoxLayout* HighLayout = new QHBoxLayout;
    const QString high_tooltip = "High frequencies gain";
    QLabel *high_label = new QLabel("High");
    high_label->setFixedWidth(label_width);
    high_slider->setMinimum(0);
    high_slider->setMaximum(200);
    high_slider->setOrientation(Qt::Horizontal);

    connect(high_slider,SIGNAL(valueChanged(int)), this, SLOT(HighChanged(int)));

    high_label->setToolTip(high_tooltip);
    high_slider->setToolTip(high_tooltip);

    HighLayout->addWidget(high_label);
    HighLayout->addWidget(high_slider);
    MainAudioSyncLayout->addLayout(HighLayout);


    QHBoxLayout* PresetLayout = new QHBoxLayout;
    const QString preset_tooltip = "Use predefined preset";
    QLabel *preset_label = new QLabel("Preset");
    preset_label->setFixedWidth(label_width);

    int devices_count = (int)AudioSyncPresets.size();
    for(int i = 0; i < (int)devices_count; i++)
    {
        preset_selector->addItem(AudioSyncPresets[i].name.c_str());
    }

    connect(preset_selector, SIGNAL(currentIndexChanged(int)), this, SLOT(PresetChanged(int)));

    preset_label->setToolTip(preset_tooltip);
    preset_selector->setToolTip(preset_tooltip);

    PresetLayout->addWidget(preset_label);
    PresetLayout->addWidget(preset_selector);
    MainAudioSyncLayout->addLayout(PresetLayout);


    QPushButton *reset_defaults_button = new QPushButton("Reset defaults");
    MainAudioSyncLayout->addWidget(reset_defaults_button);
    connect(reset_defaults_button,SIGNAL(clicked(bool)), this, SLOT(RestoreDefaultSettings()));

    AudioSyncFrame->setLayout(MainAudioSyncLayout);
    PrimaryLayout->addWidget(AudioSyncFrame);
    PrimaryFrame->setLayout(PrimaryLayout);

    setLayout(new QGridLayout(this));
    layout()->addWidget(PrimaryFrame);
}

*/


