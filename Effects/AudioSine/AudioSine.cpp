#include "AudioSine.h"
#include "ColorUtils.h"
#include "Audio/AudioManager.h"
#include "OpenRGBEffectSettings.h"

REGISTER_EFFECT(AudioSine);

AudioSine::AudioSine(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::AudioSine)
{
    ui->setupUi(this);

    EffectDetails.EffectName        = "AudioSine";
    EffectDetails.EffectClassName   = ClassName();
    EffectDetails.EffectDescription = "Sinusoidal audio rendering";
    EffectDetails.IsReversable      = true;
    EffectDetails.MaxSpeed          = 100;
    EffectDetails.MinSpeed          = 1;
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom    = false;

    SetSpeed(50);

    ui->color_mode->addItems({"Spectrum cycle", "Static"});
    ui->background->SetRGBColor(background);
    ui->wave_color->SetRGBColor(wave_color);
    ui->thickness->setValue(10);
    ui->glow->setValue(50);
    ui->color_change_speed->setValue(50);

    /*--------------------------*\
    | Setup audio                |
    \*--------------------------*/

    memcpy(&audio_settings_struct, &OpenRGBEffectSettings::globalSettings.audio_settings,sizeof(Audio::AudioSettingsStruct));

    audio_signal_processor.SetNormalization(&audio_settings_struct);

    connect(&audio_settings, &AudioSettings::AudioDeviceChanged, this, &AudioSine::OnAudioDeviceChanged);
    connect(&audio_settings, &AudioSettings::NormalizationChanged,[=]{
        audio_signal_processor.SetNormalization(&audio_settings_struct);
    });

    audio_settings.SetSettings(&audio_settings_struct);
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
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->RegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioSine::Stop()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioSine::on_color_mode_currentIndexChanged(int value)
{
    color_mode = value;
    if(color_mode == 0)
    {
        ui->wave_colorLabel->setEnabled(false);
        ui->wave_color->setEnabled(false);
    }
    else
    {
        ui->wave_colorLabel->setEnabled(true);
        ui->wave_color->setEnabled(true);
    }
}

void AudioSine::OnAudioDeviceChanged(int value)
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

void AudioSine::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    audio_signal_processor.Process(FPS, &audio_settings_struct);

    for(ControllerZone* controller_zone : controller_zones)
    {
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            for(unsigned int i = 0; i < width; i++)
            {
                float sine_value = GetSineValue(i, width, controller_zone->reverse);

                RGBColor color = GetColor(sine_value, 0, height);
                controller_zone->SetLED(i, color, Brightness);
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
                    controller_zone->SetLED(led_num, color, Brightness);
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

    for (int i = 0; i < 256; i+= audio_settings_struct.avg_size)
    {
        value +=
                height_mult *
                audio_signal_processor.Data().fft_fltr[i] *
                sin(x_percent * 0.25 * repeat * (i / (float) audio_settings_struct.avg_size) * pi);
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

void AudioSine::on_audio_settings_clicked()
{
    audio_settings.show();
}

json AudioSine::SaveCustomSettings()
{
    json settings;

    settings["color_mode"] = color_mode;
    settings["repeat"] = repeat;
    settings["glow"] = glow;
    settings["thickness"] = thickness;
    settings["oscillation"] = oscillation;
    settings["color_change_speed"] = color_change_speed;
    settings["background"] = background;
    settings["wave_color"] = wave_color;
    settings["audio_settings"] = audio_settings_struct;
    
    return settings;
}

void AudioSine::LoadCustomSettings(json Settings)
{
    if (Settings.contains("color_mode"))            ui->color_mode->setCurrentIndex(Settings["color_mode"]);
    if (Settings.contains("repeat"))                ui->repeat->setValue(Settings["repeat"]);
    if (Settings.contains("glow"))                  ui->glow->setValue(Settings["glow"]);
    if (Settings.contains("thickness"))             ui->thickness->setValue(Settings["thickness"]);
    if (Settings.contains("oscillation"))           ui->oscillation->setValue(Settings["oscillation"]);
    if (Settings.contains("color_change_speed"))    ui->color_change_speed->setValue(Settings["color_change_speed"]);

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

    if (Settings.contains("audio_settings"))
    {
        audio_settings_struct = Settings["audio_settings"];
        audio_settings.SetSettings(&audio_settings_struct);
    }
}
