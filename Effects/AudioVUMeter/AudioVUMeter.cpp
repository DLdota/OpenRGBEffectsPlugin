#include "AudioVUMeter.h"
#include "ColorUtils.h"
#include "Audio/AudioManager.h"
#include "hsv.h"
#include "OpenRGBEffectSettings.h"

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
    EffectDetails.SupportsRandom = false;

    SetSpeed(10);

    /*--------------------------*\
    | Setup audio                |
    \*--------------------------*/

    memcpy(&audio_settings_struct, &OpenRGBEffectSettings::globalSettings.audio_settings,sizeof(Audio::AudioSettingsStruct));

    audio_signal_processor.SetNormalization(&audio_settings_struct);

    connect(&audio_settings, &AudioSettings::AudioDeviceChanged, this, &AudioVUMeter::OnAudioDeviceChanged);
    connect(&audio_settings, &AudioSettings::NormalizationChanged,[=]{
        audio_signal_processor.SetNormalization(&audio_settings_struct);
    });

    audio_settings.SetSettings(&audio_settings_struct);
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
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->RegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioVUMeter::Stop()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_settings_struct.audio_device, this);
    }
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

void AudioVUMeter::on_invert_hue_stateChanged(int value)
{
    invert_hue = value;
}

void AudioVUMeter::OnAudioDeviceChanged(int value)
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

void AudioVUMeter::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    audio_signal_processor.Process(FPS, &audio_settings_struct);

    float amp = 0;

    for(int i = 0; i < 256; i += audio_settings_struct.avg_size)
    {
        amp += audio_signal_processor.Data().fft_fltr[i];
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
        zone_type ZT = controller_zone->type();
        bool reverse = controller_zone->reverse;

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(amp, reverse? width - i - 1 : i, width);
                controller_zone->SetLED(i, color, Brightness, Temperature, Tint);
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
                    controller_zone->SetLED(led_num, color, Brightness, Temperature, Tint);
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
    hsv.hue = int(color_offset + (y/h*360*color_spread*0.01))%360;

    if(invert_hue)
    {
        hsv.hue = 360 - hsv.hue;
    }

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

void AudioVUMeter::on_audio_settings_clicked()
{
    audio_settings.show();
}

json AudioVUMeter::SaveCustomSettings()
{
    json settings;

    settings["color_offset"] = color_offset;
    settings["color_spread"] = color_spread;
    settings["saturation"] = saturation;
    settings["invert_hue"] = invert_hue;
    settings["audio_settings"] = audio_settings_struct;

    return settings;
}

void AudioVUMeter::LoadCustomSettings(json Settings)
{
    if (Settings.contains("color_offset"))
        ui->color_offset->setValue(Settings["color_offset"]);

    if (Settings.contains("color_spread"))
        ui->color_spread->setValue(Settings["color_spread"]);

    if (Settings.contains("saturation"))
        ui->saturation->setValue(Settings["saturation"]);

    if (Settings.contains("invert_hue"))
        ui->invert_hue->setChecked(Settings["invert_hue"]);

    if (Settings.contains("audio_settings"))
    {
        audio_settings_struct = Settings["audio_settings"];
        audio_settings.SetSettings(&audio_settings_struct);
    }
}
