#include "AudioSync.h"


/*------------------------*\
| Standard Effect Members  |
\*------------------------*/
AudioSync::AudioSync(): QObject(nullptr)
{
    Init();
}

AudioSync::~AudioSync()
{
    Stop();
}

EffectInfo AudioSync::DefineEffectDetails()
{
    AudioSync::EffectDetails.EffectName = "Audio Sync";
    AudioSync::EffectDetails.EffectDescription = "Synchronize to master audio";

    AudioSync::EffectDetails.IsReversable = false;
    AudioSync::EffectDetails.MaxSpeed     = 0;
    AudioSync::EffectDetails.MinSpeed     = 0;
    AudioSync::EffectDetails.UserColors   = 0;

    AudioSync::EffectDetails.MaxSlider2Val = 0;
    AudioSync::EffectDetails.MinSlider2Val = 0;
    AudioSync::EffectDetails.Slider2Name   = "";

    AudioSync::EffectDetails.HasCustomWidgets = true;
    AudioSync::EffectDetails.HasCustomSettings = true;

    return AudioSync::EffectDetails;
}

void AudioSync::DefineExtraOptions(QLayout* ParentLayout)
{
    QFrame* AudioSyncFrame = new QFrame();
    QVBoxLayout* MainAudioSyncLayout = new QVBoxLayout;

    int label_width = 120;

    /*------------------*\
    | Frequency preview  |
    \*------------------*/
    graphics_view = new QGraphicsView();
    image = new QImage(256, 64, QImage::Format_RGB30);
    scene = new QGraphicsScene();
    graphics_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphics_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    graphics_view->setScene(scene);
    MainAudioSyncLayout->addWidget(graphics_view);

    /*-----------*\
    | Device list |
    \*-----------*/
    QHBoxLayout* DeviceListLayout = new QHBoxLayout;
    const QString device_tooltip = "Switch audio device";
    QLabel *device_label = new QLabel("Device");
    QComboBox* device_list_selector = new QComboBox();

    device_label->setFixedWidth(label_width);

    device_label->setToolTip(device_tooltip);
    device_list_selector->setToolTip(device_tooltip);

    std::vector<char *> devices = AudioManager::get()->GetAudioDevices();
    foreach (const char * str, AudioManager::get()->GetAudioDevices())
    {
        device_list_selector->addItem(str);
    }

    if(devices.size() > 0)
    {
        audio_device_idx = 0;
    }
    // todo : else -> hide ui and display a message that no audio device has been found

    connect(device_list_selector, SIGNAL(currentIndexChanged(int)), this, SLOT(SelectDeviceChanged(int)));

    DeviceListLayout->addWidget(device_label);
    DeviceListLayout->addWidget(device_list_selector);
    MainAudioSyncLayout->addLayout(DeviceListLayout);

    /*--------------------*\
    | Bypass band slider   |
    \*--------------------*/
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

    /*---------------*\
    | Rainbow shift   |
    \*---------------*/
    QHBoxLayout* RBShiftLayout = new QHBoxLayout;
    const QString rainbow_shift_tooltip = "Rainbow shift";
    QLabel *rainbow_shift_label = new QLabel("Shift the raimbow");
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

    /*------------------*\
    | Fade step slider   |
    \*------------------*/
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

    /*-------------*\
    | Decay slider  |
    \*-------------*/
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

    /*----------------*\
    | Avg size slider  |
    \*----------------*/
    QHBoxLayout* AVRGSizeLayout = new QHBoxLayout;
    const QString avg_size_tooltip = "How many windows to keep in memory for averaging the signal";
    QLabel *avg_size_label = new QLabel("Avg size");
    avg_size_label->setFixedWidth(label_width);
    avg_size_slider->setMinimum(1);
    avg_size_slider->setMaximum(32);
    avg_size_slider->setOrientation(Qt::Horizontal);

    connect(avg_size_slider,SIGNAL(valueChanged(int)), this, SLOT(AvgSizeChanged(int)));

    avg_size_label->setToolTip(avg_size_tooltip);
    avg_size_slider->setToolTip(avg_size_tooltip);

    AVRGSizeLayout->addWidget(avg_size_label);
    AVRGSizeLayout->addWidget(avg_size_slider);
    MainAudioSyncLayout->addLayout(AVRGSizeLayout);

    /*-------------------*\
    | Avg mode dropdown   |
    \*-------------------*/
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

    /*------------------------*\
    | Filter constant slider   |
    \*------------------------*/
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

    /*------------------*\
    | Amplitude slider   |
    \*------------------*/
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

    /*-----------*\
    | Low slider  |
    \*-----------*/
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

    /*---------------*\
    | Middle slider   |
    \*---------------*/
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

    /*-------------*\
    | High slider   |
    \*-------------*/
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

    /*-------------------*\
    | Preset dropdown     |
    \*-------------------*/
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

    /*-------------------------*\
    | Reset to defaults button  |
    \*-------------------------*/
    QPushButton *reset_defaults_button = new QPushButton("Reset defaults");
    MainAudioSyncLayout->addWidget(reset_defaults_button);
    connect(reset_defaults_button,SIGNAL(clicked(bool)), this, SLOT(RestoreDefaultSettings()));

    AudioSyncFrame->setLayout(MainAudioSyncLayout);
    ParentLayout->addWidget(AudioSyncFrame);
}

void AudioSync::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
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
        fft[i] = fft[i] * ((float(current_settings.decay) / 100.0f / (60 / FPS)));
    }

    AudioManager::get()->Capture(audio_device_idx, fft_tmp);

    #ifdef _WIN32
    for (int i = 0; i < 512; i++)
    {
        fft_tmp[i] *= (current_settings.amplitude * (amplitude_max_value - amplitude_min_value));
    }
    #else
    for (int i = 0; i < 512; i++)
    {
        fft_tmp[i] = (fft_tmp[i] - 128.0f) * ((current_settings.amplitude * (amplitude_max_value - amplitude_min_value)) / 128.0f);
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
    for (k = 0; k < current_settings.avg_size; k++)
    {
        sum1 += fft[k];
        sum2 += fft[255 - k];
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

    for(int i = 0; i < 256; i++)
    {
        fft_fltr[i] = fft_fltr[i] + (current_settings.filter_constant * (fft[i] - fft_fltr[i]));

        if(i < 64)
        {
            fft_fltr[i] *= current_settings.low;
        }
        else if ( i < 192)
        {
            fft_fltr[i] *= current_settings.middle;
        }
        else
        {
            fft_fltr[i] *= current_settings.high;
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
                image->setPixel(i, 63 - y, color);
            } else {
                image->setPixel(i, 63 - y, OFF);
            }
        }
    }

    emit UpdateGraphSignal();


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

        current_freq_sat = 255; // - 255 * pow(max_value, 9); // todo : reach white on high amplitudes
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

    // todo : insert more colors at low fps
    colors_rotation.insert(colors_rotation.begin(), color);

    while(colors_rotation.size()>1024)
    {
        colors_rotation.pop_back();
    }

    int colors_count = (int)colors_rotation.size();

    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
        {
            /*-------------------*\
            | Setup for the loop  |
            \*-------------------*/
            int SetLEDIndex = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].start_idx;
            zone_type ZT = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].type;

            /*----------------------------------------------------*\
            | Adjust how it applies for the specific type of zone  |
            \*----------------------------------------------------*/
            if (ZT == ZONE_TYPE_SINGLE)
            {
                int led_count = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;
                for (int LedID = 0; LedID < led_count; LedID++)
                {
                    Controllers[ControllerID].Controller->SetLED(SetLEDIndex + LedID, colors_rotation[0]);
                }
            }
            else if (ZT == ZONE_TYPE_LINEAR)
            {
                int led_count = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].leds_count;

                for (int LedID = 0; LedID < led_count && LedID < colors_count; LedID++)
                {
                    Controllers[ControllerID].Controller->SetLED((SetLEDIndex+LedID), colors_rotation[LedID]);
                }
            }

            else if (ZT == ZONE_TYPE_MATRIX)
            {
                int cols = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->width;
                int rows = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->height;


                for (int col_id = 0; col_id < cols && col_id < colors_count; col_id++)
                {

                    for (int row_id = 0; row_id < rows; row_id++)
                    {
                        int LedID = Controllers[ControllerID].Controller->zones[Controllers[ControllerID].OwnedZones[ZoneID]].matrix_map->map[((row_id * cols) + col_id)];
                        Controllers[ControllerID].Controller->SetLED(SetLEDIndex + LedID, colors_rotation[col_id]);
                    }
                }
            }

        }
    }

}

void AudioSync::LoadCustomSettings(json Settings)
{
    if (Settings.contains("fade_step"))           current_settings.fade_step       = Settings["fade_step"];
    if (Settings.contains("rainbow_shift"))       current_settings.rainbow_shift   = Settings["rainbow_shift"];
    if (Settings.contains("bypass_min"))          current_settings.bypass_min      = Settings["bypass_min"];
    if (Settings.contains("bypass_max"))          current_settings.bypass_max      = Settings["bypass_max"];
    if (Settings.contains("avg_size"))            current_settings.avg_size        = Settings["avg_size"];
    if (Settings.contains("avg_mode"))            current_settings.avg_mode        = Settings["avg_mode"];
    if (Settings.contains("decay"))               current_settings.decay           = Settings["decay"];
    if (Settings.contains("filter_constant"))     current_settings.filter_constant = Settings["filter_constant"];
    if (Settings.contains("high"))                current_settings.high            = Settings["high"];
    if (Settings.contains("middle"))              current_settings.middle          = Settings["middle"];
    if (Settings.contains("low"))                 current_settings.low             = Settings["low"];
    if (Settings.contains("amplitude"))           current_settings.amplitude       = Settings["amplitude"];
    if (Settings.contains("amplitude_min_value")) amplitude_min_value              = Settings["amplitude_min_value"];
    if (Settings.contains("amplitude_max_value")) amplitude_max_value              = Settings["amplitude_max_value"];

    UpdateUiSettings();
    return;
}

json AudioSync::SaveCustomSettings(json Settings)
{
    Settings["fade_step"]       = current_settings.fade_step;
    Settings["rainbow_shift"]   = current_settings.rainbow_shift;
    Settings["bypass_min"]      = current_settings.bypass_min;
    Settings["bypass_max"]      = current_settings.bypass_max;
    Settings["avg_size"]        = current_settings.avg_size;
    Settings["avg_mode"]        = current_settings.avg_mode;
    Settings["decay"]           = current_settings.decay;
    Settings["filter_constant"] = current_settings.filter_constant;
    Settings["high"]            = current_settings.high;
    Settings["middle"]          = current_settings.middle;
    Settings["low"]             = current_settings.middle;

    Settings["amplitude_min_value"]   = amplitude_min_value;
    Settings["amplitude_max_value"]   = amplitude_max_value;
    Settings["amplitude"]       = current_settings.amplitude;

    return Settings;
}

void AudioSync::EffectState(const bool State)
{
    State ? Start() : Stop();
}


/*-------------------------------------------------------------------*\
| Function in charge of changing values based on the value passed in  |
\*-------------------------------------------------------------------*/
void AudioSync::SelectDeviceChanged(int idx)
{
    bool was_running = is_running;

    if(is_running)
    {
        Stop();
    }

    audio_device_idx = idx;

    if(was_running)
    {
        Start();
    }
}

void AudioSync::RainbowShiftChanged(int value)
{    
    current_settings.rainbow_shift = value;
}

void AudioSync::FadeStepChanged(int value)
{
    current_settings.fade_step = value;
}

void AudioSync::AvgSizeChanged(int value)
{
    current_settings.avg_size = value;
}

void AudioSync::AvgModeChanged(int value)
{
    current_settings.avg_mode = value;
}

void AudioSync::DecayChanged(int value)
{
    current_settings.decay = value;
}

void AudioSync::BypassChanged(int min, int max)
{
    current_settings.bypass_min = min;
    current_settings.bypass_max = max;
}

void AudioSync::FilterConstantChanged(int value)
{
    current_settings.filter_constant = value / 100.0f;
}

void AudioSync::AmplitudeChanged(int value)
{
    current_settings.amplitude = value / 100.0f;
}

void AudioSync::LowChanged(int value)
{
    current_settings.low = value / 100.0f;
}

void AudioSync::MiddleChanged(int value)
{
    current_settings.middle = value / 100.0f;
}

void AudioSync::HighChanged(int value)
{
    current_settings.high = value / 100.0f;
}

void AudioSync::PresetChanged(int idx)
{
    /*-----------------------------------------*\
    | Read the preset and call Update function  |
    \*-----------------------------------------*/
    current_settings = AudioSyncSettings(AudioSyncPresets[idx]);
    UpdateUiSettings();
}


/*------------------------*\
| Miscellaneous functions  |
\*------------------------*/
void AudioSync::ToggleAmplitudeChangeInputs()
{
    /*-------------------------------------------------------------------------------------------------------------*\
    | Why is it done like this? Wouldn't it be easier to read the text off of the button? Or assign a value to it   |
    |                                                                                                               |
    | If it is Showing: Do a bounds check and apply new min and max values, Then set the button text                |
    | If it isn't Showing: Show text boxes and change button text                                                   |
    \*-------------------------------------------------------------------------------------------------------------*/
    if(amplitude_slider_min->isVisible())
    {
        if(amplitude_slider_min->value() < amplitude_slider_max->value())
        {
            amplitude_slider_min->hide();
            amplitude_slider_max->hide();

            amplitude_min_value = amplitude_slider_min->value();
            amplitude_max_value = amplitude_slider_max->value();


            UpdateUiSettings();

            change_amplitude_button->setText("Edit");
        }
    }
    else
    {
        amplitude_slider_min->show();
        amplitude_slider_max->show();
        change_amplitude_button->setText("Apply");
    }
}

void AudioSync::UpdateGraph()
{
    /*----------------------------------*\
    | Set bounds for drawing visualizer  |
    \*----------------------------------*/
    scene->setSceneRect(0,0,graphics_view->width(),graphics_view->height());

    QRectF bounds = scene->itemsBoundingRect();
    bounds.setWidth(bounds.width());
    bounds.setHeight(bounds.height());

    graphics_view->fitInView(bounds, Qt::IgnoreAspectRatio);
    graphics_view->centerOn(0, 0);

    /*----------------------------------*\
    | Scale image to fit preview window  |
    \*----------------------------------*/
    image->scaled(graphics_view->width(), graphics_view->height(), Qt::IgnoreAspectRatio);

    /*--------------------------------------------------------*\
    | Convert to image, Wipe previous pixmap, Set the new one  |
    \*--------------------------------------------------------*/
    pixmap.convertFromImage(*image);
    scene->clear();
    scene->addPixmap(pixmap);
}

void AudioSync::RestoreDefaultSettings()
{
    /*-------------------------------------------------*\
    | Set Settings to defualt and call update function  |
    \*-------------------------------------------------*/
    current_settings = AudioSyncSettings(AudioSyncPresets[0]);
    preset_selector->setCurrentIndex(0);
    UpdateUiSettings();
}

void AudioSync::UpdateUiSettings()
{
    /*-----------------------------------------------*\
    | Set the min and max values of all GUI elements  |
    \*-----------------------------------------------*/
    bypass_slider->setMinimum(0);
    bypass_slider->setMaximum(256);
    bypass_slider->setValues(current_settings.bypass_min, current_settings.bypass_max);
    rainbow_shift_slider->setValue(current_settings.rainbow_shift);
    fade_step_slider->setValue(current_settings.fade_step);
    decay_slider->setValue(current_settings.decay);
    avg_size_slider->setValue(current_settings.avg_size);
    avg_mode_selector->setCurrentIndex(current_settings.avg_mode);
    filter_constant_slider->setValue(current_settings.filter_constant * 100);
    low_slider->setValue(current_settings.low*100);
    middle_slider->setValue(current_settings.middle*100);
    high_slider->setValue(current_settings.high*100);

    amplitude_slider_min->setValue(amplitude_min_value);
    amplitude_slider_max->setValue(amplitude_max_value);
    amplitude_slider->setValue(current_settings.amplitude * 100);
}

void AudioSync::Init()
{
    is_running = false;

    /*-----------------------------------------------------------*\
    | no device selected yet, waiting for settings to be loaded.  |
    \*-----------------------------------------------------------*/
    audio_device_idx = -1;

    /*-------------------------------------------------*\
    | Create empty versions of all of the GUI elements  |
    \*-------------------------------------------------*/
    bypass_slider = new ctkRangeSlider(Qt::Horizontal);
    rainbow_shift_slider = new QSlider();
    high_slider = new QSlider();
    middle_slider = new QSlider();
    low_slider = new QSlider();
    amplitude_slider = new QSlider();
    fade_step_slider = new QSlider();
    decay_slider = new QSlider();
    avg_size_slider = new QSlider();
    avg_mode_selector = new QComboBox();
    filter_constant_slider = new QSlider();
    preset_selector = new QComboBox();
    amplitude_slider_min = new QSpinBox();
    amplitude_slider_max = new QSpinBox();

    RestoreDefaultSettings();

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

    int start_hue = 360;
    int stop_hue =  0;

    float hue_step = (stop_hue-start_hue)/256.0f;

    /*---------------------------------------------------------------------------------*\
    | Create a static list of colors to read off of when drawing                        |
    | Using more memory is better than having to render 256 colors with CPU every cycle |
    \*---------------------------------------------------------------------------------*/
    for(int i = 0; i<256;i++)
    {
        rainbow_hues.push_back(ceil(start_hue + i * hue_step));
    }

    /*--------------------------*\
    | Map signal to UpdateGraph  |
    \*--------------------------*/
    connect(this, SIGNAL(UpdateGraphSignal()), this, SLOT(UpdateGraph()));
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
