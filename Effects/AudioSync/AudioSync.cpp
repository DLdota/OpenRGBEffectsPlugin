#include "AudioSync.h"
#include "math.h"
#include "ColorUtils.h"
#include "Audio/AudioManager.h"
#include "ui_AudioSync.h"
#include "OpenRGBEffectSettings.h"

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
    EffectDetails.SupportsRandom = false;

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
    ui->bypass->setMinimumPosition(0);
    ui->bypass->setMaximumPosition(256);

    ui->color_fade_speed->setValue(50);
    ui->hue_shift->setValue(0);

    ui->silent_color->setChecked(false);
    ui->silent_color_value->SetRGBColor(0);
    ui->silent_color_value->setVisible(false);

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

    /*--------------------------*\
    | Setup audio                |
    \*--------------------------*/

    memcpy(&audio_settings_struct, &OpenRGBEffectSettings::globalSettings.audio_settings,sizeof(Audio::AudioSettingsStruct));

    audio_signal_processor.SetNormalization(&audio_settings_struct);

    connect(&audio_settings, &AudioSettings::AudioDeviceChanged, this, &AudioSync::OnAudioDeviceChanged);
    connect(&audio_settings, &AudioSettings::NormalizationChanged,[=]{
        audio_signal_processor.SetNormalization(&audio_settings_struct);
    });

    audio_settings.SetSettings(&audio_settings_struct);

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
    delete ui;
}

void AudioSync::EffectState(const bool state)
{
    EffectEnabled = state;
    state ? Start() : Stop();
}

void AudioSync::Start()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->RegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioSync::Stop()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioSync::OnAudioDeviceChanged(int value)
{
    bool was_running = EffectEnabled;

    if(EffectEnabled)
    {
        Stop();
    }

    audio_settings_struct.audio_device = value;

    if(was_running)
    {
        Start();
    }
}

void AudioSync::StepEffect(std::vector<ControllerZone*> controller_zones)
{    
    audio_signal_processor.Process(FPS, &audio_settings_struct);

    hsv_t HSVPixel;

    QImage image(256, 64, QImage::Format_RGB30);
    QPixmap pixmap;

    for(int i = 0; i < 256; i++)
    {
        int shifted = (i+hue_shift)%rainbow_hues.size();
        HSVPixel.hue = rainbow_hues[shifted];

        HSVPixel.saturation = (i >= bypass_min && i <= bypass_max) ? 255 : 128;
        HSVPixel.value = (i >= bypass_min && i <= bypass_max) ? 255 : 128;

        unsigned int color_value = hsv2rgb(&HSVPixel);

        RGBColor color = ToRGBColor(RGBGetBValue(color_value), RGBGetGValue(color_value), RGBGetRValue(color_value));

        float value = audio_signal_processor.Data().fft_fltr[i] * 100;

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

    for(int i = bypass_min; i < bypass_max; i++)
    {
        if(audio_signal_processor.Data().fft_fltr[i] > max_value )
        {
            max_value = audio_signal_processor.Data().fft_fltr[i];
            max_idx = i;
        }
    }

    hsv_t HSVVal;

    if(max_idx >= bypass_min && max_idx <= bypass_max)
    {
        int shifted = (max_idx+hue_shift)%rainbow_hues.size();
        immediate_freq_hue = rainbow_hues[shifted];

        // slowly reach immediate hue
        if(current_freq_hue < immediate_freq_hue)
        {
            current_freq_hue += ((immediate_freq_hue - current_freq_hue) / (1.0f - (fade_step / 100.f))) / FPS;
        }
        else if(current_freq_hue > immediate_freq_hue)
        {
            current_freq_hue -= ((current_freq_hue - immediate_freq_hue ) / (1.0f - (fade_step / 100.f)) )/ FPS;
        }

        if(saturation_mode == SATURATE_HIGH_AMPLITUDES)
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
        else if(saturation_mode == B_W_MODE)
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

    if(color == 0 && silent_color)
    {
        silent_color_timer++;
        silent_color_timer = std::min<unsigned int>(silent_color_timer, SILENT_COLOR_TIMEOUT);
        color = ColorUtils::Enlight(silent_color_value, float(silent_color_timer)/float(SILENT_COLOR_TIMEOUT));
    }
    else
    {
        silent_color_timer = 0;
    }

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
                controller_zone->SetLED(LedID, GetColor(1, reverse ? leds_count - LedID - 1 : LedID, leds_count, 1), Brightness, Temperature, Tint);
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
                    int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                    controller_zone->SetLED(LedID, GetColor(reverse ? rows - row_id - 1: row_id, reverse ? cols - col_id - 1: col_id, rows, cols), Brightness, Temperature, Tint);
                }
            }
        }
    }
}

RGBColor AudioSync::GetColor(int row, int col, int zone_width, int zone_height)
{
    switch(roll_mode)
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

void AudioSync::UpdateGraph(QPixmap pixmap)
{
    if(EffectEnabled)
    {
        ui->preview->setPixmap(pixmap);
        ui->preview->setScaledContents(true);
    }
}


/*-----------------------------------------------------*\
| Load/Save json                                        |
\*-----------------------------------------------------*/
void AudioSync::LoadCustomSettings(json settings)
{
    if (settings.contains("fade_step"))           ui->color_fade_speed->setValue(settings["fade_step"]);
    if (settings.contains("rainbow_shift"))       ui->hue_shift->setValue(settings["rainbow_shift"]);
    if (settings.contains("bypass_min"))          ui->bypass->setMinimumValue(settings["bypass_min"]);
    if (settings.contains("bypass_max"))          ui->bypass->setMaximumValue(settings["bypass_max"]);
    if (settings.contains("saturation_mode"))     ui->saturation->setCurrentIndex(settings["saturation_mode"]);
    if (settings.contains("roll_mode"))           ui->roll_mode->setCurrentIndex(settings["roll_mode"]);
    if (settings.contains("silent_color"))        ui->silent_color->setChecked(settings["silent_color"]);
    if (settings.contains("silent_color_value"))  ui->silent_color_value->SetRGBColor(settings["silent_color_value"]);

    if (settings.contains("audio_settings"))
    {
        audio_settings_struct = settings["audio_settings"];
        audio_settings.SetSettings(&audio_settings_struct);
    }

    return;
}

json AudioSync::SaveCustomSettings()
{
    json settings;


    settings["fade_step"]           = fade_step;
    settings["hue_shift"]           = hue_shift;
    settings["bypass_min"]          = bypass_min;
    settings["bypass_max"]          = bypass_max;
    settings["roll_mode"]           = roll_mode;
    settings["saturation_mode"]     = saturation_mode;
    settings["audio_settings"]      = audio_settings_struct;
    settings["silent_color"]        = silent_color;
    settings["silent_color_value"]  = silent_color_value;

    return settings;
}

/*-----------------------------------------------------*\
| UI bindings                                           |
\*-----------------------------------------------------*/
void AudioSync::on_color_fade_speed_valueChanged(int value)
{
    fade_step = value;
}

void AudioSync::on_hue_shift_valueChanged(int value)
{
    hue_shift = value;
}

void AudioSync::on_bypass_valuesChanged(int min,int max)
{
    bypass_min = min;
    bypass_max = max;
}

void AudioSync::on_saturation_currentIndexChanged(int value)
{
    saturation_mode = value;
}

void AudioSync::on_roll_mode_currentIndexChanged(int value)
{
    roll_mode = value;
}

void AudioSync::on_audio_settings_clicked()
{
    audio_settings.show();
}

void AudioSync::on_silent_color_stateChanged(int value)
{
    ui->silent_color_value->setVisible(value);
    silent_color = value;
}

void AudioSync::on_silent_color_value_ColorSelected(QColor c)
{
    silent_color_value = ColorUtils::fromQColor(c);
}
